/*
 * Copyright 2019 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <iostream>
#include <Utils/Logging/Logger.h>

#include "Utils/SharedBuffer/BufferLayout.h"

/// String to identify log entries originating from this file.
static const std::string TAG("BufferLayout");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
BufferLayout::BufferLayout(std::shared_ptr<Buffer> buffer)
	:m_buffer{buffer},
	m_readerEnabledArray{nullptr},
	m_readerCursorArray{nullptr},
	m_readerCloseIndexArray{nullptr},
	m_dataSize{0},
	m_data{nullptr} {

}

BufferLayout::~BufferLayout() {
    detach();
}

BufferLayout::Header* BufferLayout::getHeader() const {
    return reinterpret_cast<BufferLayout::Header*>(m_buffer->data());
}

std::atomic<bool>* BufferLayout::getReaderEnabledArray() const {
    return m_readerEnabledArray;
}

std::atomic<uint64_t>* BufferLayout::getReaderCursorArray() const {
    return m_readerCursorArray;
}

std::atomic<uint64_t>* BufferLayout::getReaderCloseIndexArray() const {
    return m_readerCloseIndexArray;
}

BufferLayout::Index BufferLayout::getDataSize() const {
    return m_dataSize;
}

uint8_t* BufferLayout::getData(BufferLayout::Index at) const {
    return m_data + (at % getDataSize()) * getHeader()->wordSize;
}

bool BufferLayout::init(size_t wordSize, size_t maxReaders) {
    // Make sure parameters are not too large to store.
    if (wordSize > std::numeric_limits<decltype(Header::wordSize)>::max()) {
		AISDK_ERROR(LX("initFailed").d("reason", "wordSizeTooLarge")
									.d("wordSize", wordSize)
									.d("wordSizeLimit", std::numeric_limits<decltype(Header::wordSize)>::max()));
        return false;
    }
    if (maxReaders > std::numeric_limits<decltype(Header::maxReaders)>::max()) {
		AISDK_ERROR(LX("initFailed").d("reason", "maxReadersTooLarge")
									.d("maxReaders", maxReaders)
									.d("maxReadersLimit", std::numeric_limits<decltype(Header::maxReaders)>::max()));		
        return false;
    }

    // Pre-calculate some pointers and sizes that are frequently accessed.
    calculateAndCacheConstants(wordSize, maxReaders);

	/** 
	 * We will use placement new to alloc space.
	*/
    // Default construction of the Header.
    auto header = new (getHeader()) Header;

    // Default construction of the reader arrays.
    size_t id;
    for (id = 0; id < maxReaders; ++id) {
        new (m_readerEnabledArray + id) std::atomic<bool>;
        new (m_readerCursorArray + id) std::atomic<uint64_t>;
        new (m_readerCloseIndexArray + id) std::atomic<uint64_t>;
    }

    // Header field initialization.
    header->wordSize = wordSize;
    header->maxReaders = maxReaders;
    header->isWriterEnabled = false;
    header->hasWriterBeenClosed = false;
    header->writeStartCursor = 0;
    header->writeEndCursor = 0;
    header->referenceCount = 1;

    // Reader arrays initialization.
    for (id = 0; id < maxReaders; ++id) {
        m_readerEnabledArray[id] = false;
        m_readerCursorArray[id] = 0;
        m_readerCloseIndexArray[id] = 0;
    }

    return true;
}

//attach

void BufferLayout::detach() {
    if (!isAttached()) {
        return;
    }

    auto header = getHeader();
    std::lock_guard<std::mutex> lock(header->attachMutex);
    --header->referenceCount;
    if (header->referenceCount > 0) {
        return;
    }

    // Destruction of reader arrays.
	using AtomicBool = std::atomic<bool>;
	using AtomicIndex = std::atomic<uint64_t>;
    for (size_t id = 0; id < header->maxReaders; ++id) {
        m_readerCloseIndexArray[id].~AtomicIndex();
        m_readerCursorArray[id].~AtomicIndex();
        m_readerEnabledArray[id].~AtomicBool();
    }

    // Destruction of the Header.
    header->~Header();
}

bool BufferLayout::isReaderEnabled(size_t id) const {
    return m_readerEnabledArray[id];
}

void BufferLayout::enableReaderLocked(size_t id) {
    m_readerEnabledArray[id] = true;
}

void BufferLayout::disableReaderLocked(size_t id) {
    m_readerEnabledArray[id] = false;
}

BufferLayout::Index BufferLayout::wordsUntilWrap(Index after) const {
    return alignSizeTo(after, getDataSize()) - after;
}

// Integer multiple upgrade
size_t BufferLayout::alignSizeTo(size_t size, size_t align) {
    if (size) {
        return (((size - 1) / align) + 1) * align;
    } else {
        return 0;
    }
}

/**
 * Circular buffer storage format:
 * | ---  |	------------------ 	 ------------------   ----------------------   ------------- |
 * |Header|n*ReaderEnabledArray|+n*ReaderCursorArray|+n*ReaderCloseIndexArray|raw data stream|
 * | ---  |	------------------   ------------------	  ----------------------   ------------- |
*/
size_t BufferLayout::calculateDataOffset(size_t wordSize, size_t maxReaders) {
	return alignSizeTo(calculateReaderCloseIndexArrayOffset(maxReaders) + (maxReaders * sizeof(std::atomic<uint64_t>)), wordSize);
}

// calculate skip header size as readerenableArray first position
size_t BufferLayout::calculateReaderEnabledArrayOffset() {
    return alignSizeTo(sizeof(Header), alignof(std::atomic<bool>));
}

// calculate readerCursorArray first position
size_t BufferLayout::calculateReaderCursorArrayOffset(size_t maxReaders) {
    return alignSizeTo(calculateReaderEnabledArrayOffset() + (maxReaders * sizeof(std::atomic<bool>)), alignof(std::atomic<uint64_t>));
}

size_t BufferLayout::calculateReaderCloseIndexArrayOffset(size_t maxReaders) {
    return calculateReaderCursorArrayOffset(maxReaders) + (maxReaders * sizeof(std::atomic<uint64_t>));
}

void BufferLayout::calculateAndCacheConstants(size_t wordSize, size_t maxReaders) {
    auto buffer = reinterpret_cast<uint8_t*>(m_buffer->data());
    m_readerEnabledArray = reinterpret_cast<std::atomic<bool>*>(buffer + calculateReaderEnabledArrayOffset());
    m_readerCursorArray = reinterpret_cast<std::atomic<uint64_t>*>(buffer + calculateReaderCursorArrayOffset(maxReaders));
    m_readerCloseIndexArray = reinterpret_cast<std::atomic<uint64_t>*>(buffer + calculateReaderCloseIndexArrayOffset(maxReaders));
	// 大小以字为单位
    m_dataSize = (m_buffer->size() - calculateDataOffset(wordSize, maxReaders)) / wordSize;
    m_data = buffer + calculateDataOffset(wordSize, maxReaders);
}

bool BufferLayout::isAttached() const {
    return m_data != nullptr;
}

}	// namespace sharedbuffer
}	//utils
} // namespace aisdk
