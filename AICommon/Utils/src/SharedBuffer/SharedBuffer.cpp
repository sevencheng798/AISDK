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
#include <Utils/SharedBuffer/SharedBuffer.h>

/// String to identify log entries originating from this file.
static const std::string TAG("SharedBuffer");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
std::unique_ptr<SharedBuffer> SharedBuffer::create(
	std::shared_ptr<Buffer> buffer,
	size_t wordSize,
	size_t maxReaders) {
	size_t expectedSize = calculateBufferSize(1, wordSize, maxReaders);
	if (0 == expectedSize) {
		// Logged in calcutlateBuffersize().
		return nullptr;
	} else if (nullptr == buffer) {
		AISDK_ERROR(LX("createFailed").d("reason", "nullBuffer"));
		return nullptr;
	} else if (expectedSize > buffer->size()) {
		AISDK_ERROR(LX("createFailed")
						   .d("reason", "bufferSizeTooSmall")
						   .d("bufferSize", buffer->size())
						   .d("expectedSize", expectedSize));
		return nullptr;
	}

	std::unique_ptr<SharedBuffer> sds(new SharedBuffer(buffer));
	if (!sds->m_bufferLayout->init(wordSize, maxReaders)) {
		// Logged in init().
		return nullptr;
	}
	return sds;
}

size_t SharedBuffer::calculateBufferSize(size_t nWords, size_t wordSize, size_t maxReaders) {
    if (0 == nWords) {
        AISDK_ERROR(LX("calculateBufferSizeFailed").d("reason", "numWordsZero"));
        return 0;
    } else if (0 == wordSize) {
       	AISDK_ERROR(LX("calculateBufferSizeFailed").d("reason", "wordSizeZero"));
        return 0;
    }
	// Get the all heads size of @c BufferLayout. 
    size_t overhead = BufferLayout::calculateDataOffset(wordSize, maxReaders);
    size_t dataSize = nWords * wordSize;
    return overhead + dataSize;
}

size_t SharedBuffer::getMaxReaders() const {
    return m_bufferLayout->getHeader()->maxReaders;
}

SharedBuffer::Index SharedBuffer::getDataSize() const {
    return m_bufferLayout->getDataSize();
}

size_t SharedBuffer::getWordSize() const {
    return m_bufferLayout->getHeader()->wordSize;
}

std::unique_ptr<Writer> SharedBuffer::createWriter(
    Writer::Policy policy,
    bool forceReplacement) {
    auto header = m_bufferLayout->getHeader();
    std::lock_guard<std::mutex> lock(header->writerEnableMutex);
    if (header->isWriterEnabled && !forceReplacement) {
        AISDK_ERROR(LX("createWriterFailed")
                       .d("reason", "existingWriterAttached")
                       .d("forceReplacement", "false"));
        return nullptr;
    } else {
        return std::unique_ptr<Writer>(new Writer(policy, m_bufferLayout));
    }  
}

std::unique_ptr<Reader> SharedBuffer::createReader(
    Reader::Policy policy,
    bool startWithNewData) {
    std::unique_lock<std::mutex> lock(m_bufferLayout->getHeader()->readerEnableMutex);
    for (size_t id = 0; id < m_bufferLayout->getHeader()->maxReaders; ++id) {
        if (!m_bufferLayout->isReaderEnabled(id)) {
            return createReaderLocked(id, policy, startWithNewData, false, &lock);
        }
    }
    AISDK_ERROR(LX("createWriterFailed").d("reason", "noAvailableReaders"));
    return nullptr;
}

SharedBuffer::SharedBuffer(std::shared_ptr<Buffer> buffer):
	m_bufferLayout{std::make_shared<BufferLayout>(buffer)} {
}

SharedBuffer::~SharedBuffer() {
	AISDK_INFO(LX("~SharedBuffer").d("reason", "destory"));

}

std::unique_ptr<Reader> SharedBuffer::createReaderLocked(
	size_t id,
	Reader::Policy policy,
	bool startWithNewData,
	bool forceReplacement,
	std::unique_lock<std::mutex>* lock) {
	if (m_bufferLayout->isReaderEnabled(id) && !forceReplacement) {
		AISDK_ERROR(LX("createReaderLockedFailed")
						.d("reason", "readerAlreadyAttached")
						.d("readerId", id)
						.d("forceReplacement", "false"));
		return nullptr;
	} else {
		// Note: Reader constructor does not call updateUnconsumedCursor() automatically, because we may be seeking to
		// a blocked writer's cursor below (if !startWithNewData), and we don't want the writer to start moving before
		// we seek.
		auto reader = std::unique_ptr<Reader>(new Reader(policy, m_bufferLayout, id));
		lock->unlock();
#if 0
		if (startWithNewData) {
			// We're not moving the cursor again, so call updateUnconsumedCursor() now.
			m_bufferLayout->updateOldestUnconsumedCursor();
		} else {
			Index offset = m_bufferLayout->getDataSize();
			if (m_bufferLayout->getHeader()->writeStartCursor < offset) {
				offset = m_bufferLayout->getHeader()->writeStartCursor;
			}
			// Note: seek() will call updateUnconsumedCursor().
			if (!reader->seek(offset, Reader::Reference::BEFORE_WRITER)) {
				// Logged in seek().
				return nullptr;
			}
		}
#else
	if(!startWithNewData) {
		Index offset = m_bufferLayout->getDataSize();
		if (m_bufferLayout->getHeader()->writeStartCursor < offset) {
			offset = m_bufferLayout->getHeader()->writeStartCursor;
		}
		// Note: seek() will call updateUnconsumedCursor().
		if (!reader->seek(offset, Reader::Reference::BEFORE_WRITER)) {
			// Logged in seek().
			return nullptr;
		}
	}
#endif
		return reader;
	}
}


}	// namespace sharedbuffer
}	//utils
} // namespace aisdk
