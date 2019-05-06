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

#include <Utils/Logging/Logger.h> 
#include <Utils/SharedBuffer/Writer.h>

#include <string>
#include <cstring>

/// String to identify log entries originating from this file.
static const std::string TAG{"SBWriter"};

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
Writer::Writer(Policy policy, std::shared_ptr<BufferLayout> bufferLayout) :
	m_policy{policy},
	m_bufferLayout{bufferLayout},
	m_closed{false} {
	// Note - SharedBuffer::createWriter() holds writerEnableMutex while calling this function.
	auto header = m_bufferLayout->getHeader();
	// Enable the @c Writer.
	header->isWriterEnabled = true;
	header->writeEndCursor = header->writeStartCursor.load();
}

Writer::~Writer() {
	AISDK_INFO(LX("~Writer").d("reason", "destory"));
    close();
}

ssize_t Writer::write(const void* buf, size_t nWords, std::chrono::milliseconds timeout) {
    if (nullptr == buf) {
        AISDK_ERROR(LX("writeFailed").d("reason", "nullBuffer"));
        return Error::INVALID;
    }
    if (0 == nWords) {
        AISDK_ERROR(LX("writeFailed").d("reason", "zeroNumWords"));
        return Error::INVALID;
    }

    auto header = m_bufferLayout->getHeader();
    if (!header->isWriterEnabled) {
        AISDK_ERROR(LX("writeFailed").d("reason", "writerDisabled"));
        return Error::CLOSED;
    }

    auto wordsToCopy = nWords;
    auto buf8 = static_cast<const uint8_t*>(buf);
    std::unique_lock<std::mutex> backwardSeekLock(header->backwardSeekMutex, std::defer_lock);
    BufferLayout::Index writeEnd = header->writeStartCursor + nWords;

    switch (m_policy) {
        case Policy::NONBLOCKABLE:
            // For NONBLOCKABLE, we can truncate the write if it won't fit in the buffer.
            if (nWords > m_bufferLayout->getDataSize()) {
                wordsToCopy = nWords = m_bufferLayout->getDataSize();
                writeEnd = header->writeStartCursor + nWords;
            }
            break;
        case Policy::BLOCKING:
            // For BLOCKING, we need to wait until there is room for at least one word.
            backwardSeekLock.lock();
			// To-Do for future.
            break;
    }

    header->writeEndCursor = writeEnd;

    // We've updated our end cursor, so we no longer need to hold off backward seeks.
    if (backwardSeekLock) {
        backwardSeekLock.unlock();
    }

    // Split it across the wrap.
    size_t beforeWrap = m_bufferLayout->wordsUntilWrap(header->writeStartCursor);
    if (beforeWrap > wordsToCopy) {
        beforeWrap = wordsToCopy;
    }
    size_t afterWrap = wordsToCopy - beforeWrap;

    // Copy the two segments.
    memcpy(m_bufferLayout->getData(header->writeStartCursor), buf8, beforeWrap * getWordSize());
    if (afterWrap > 0) {
        memcpy(
            m_bufferLayout->getData(header->writeStartCursor + beforeWrap),
            buf8 + beforeWrap * getWordSize(),
            afterWrap * getWordSize());
    }

	// For BLOCKING need to lock and notify Reader
	// Note: as an optimization, we could skip this if there are no blocking readers
    std::unique_lock<std::mutex> dataAvailableLock(header->dataAvailableMutex, std::defer_lock);
    if (Policy::NONBLOCKABLE != m_policy) {
        dataAvailableLock.lock();
    }
    header->writeStartCursor = header->writeEndCursor.load();
    if (Policy::NONBLOCKABLE != m_policy) {
        dataAvailableLock.unlock();
    }

    // Notify the reader(s).
    // Note: as an optimization, we could skip this if there are no blocking readers.
    header->dataAvailableConditionVariable.notify_all();

    return nWords;
}

BufferLayout::Index Writer::tell() const {
    return m_bufferLayout->getHeader()->writeStartCursor;
}

void Writer::close() {
    auto header = m_bufferLayout->getHeader();
    std::lock_guard<std::mutex> lock(header->writerEnableMutex);
    if (m_closed) {
        return;
    }
    if (header->isWriterEnabled) {
        header->isWriterEnabled = false;

        std::unique_lock<std::mutex> dataAvailableLock(header->dataAvailableMutex);

        header->hasWriterBeenClosed = true;

        header->dataAvailableConditionVariable.notify_all();
    }
    m_closed = true;
}

size_t Writer::getWordSize() const {
    return m_bufferLayout->getHeader()->wordSize;
}

#if 0
std::string Writer::errorToString(Error error) {
#if 0
    switch (error) {
        case Error::CLOSED:
            return "CLOSED";
        case Error::WOULDBLOCK:
            return "WOULDBLOCK";
        case Error::INVALID:
            return "INVALID";
        case Error::TIMEDOUT:
            return "TIMEDOUT";
    }
#else
	if(error == Error::CLOSED)
		return "CLOSED";
#endif
    AISDK_ERROR(LX("errorToStringFailed").d("reason", "invalidError").d("error", error));
    return "(unknown error " + std::to_string(error) + ")";
}
#endif
}	// namespace sharedbuffer
}	//utils
} // namespace aisdk
