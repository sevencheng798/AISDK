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
#include <cstring> 
#include <Utils/Logging/Logger.h>
#include <Utils/SharedBuffer/Reader.h>

/// String to identify log entries originating from this file.
static const std::string TAG{"SBReader"};

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
Reader::Reader(Policy policy, std::shared_ptr<BufferLayout> bufferLayout, uint8_t id) :
        m_policy{policy},
        m_bufferLayout{bufferLayout},
        m_id{id},
        m_readerCursor{&m_bufferLayout->getReaderCursorArray()[m_id]},
        m_readerCloseIndex{&m_bufferLayout->getReaderCloseIndexArray()[m_id]} {
    // Note - SharedBuffer::createReader() holds readerEnableMutex while calling this function.
    // Read only new data from current up-to-date write cursor.
    *m_readerCursor = m_bufferLayout->getHeader()->writeStartCursor.load();

    // Read indefinitely - default set max value.
    *m_readerCloseIndex = std::numeric_limits<BufferLayout::Index>::max();

    m_bufferLayout->enableReaderLocked(m_id);
}

Reader::~Reader() {
	/// Sync WriterStartCursor and ReaderCursor.
	AISDK_INFO(LX("~Reader").d("reason", "destory"));
    seek(0, Reference::BEFORE_WRITER);

    std::lock_guard<std::mutex> lock(m_bufferLayout->getHeader()->readerEnableMutex);
    m_bufferLayout->disableReaderLocked(m_id);
}

ssize_t Reader::read(void* buf, size_t nWords, std::chrono::milliseconds timeout) {
    if (nullptr == buf) {
        AISDK_ERROR(LX("readFailed").d("reason", "nullBuffer"));
        return Error::INVALID;
    }

    if (0 == nWords) {
        AISDK_ERROR(LX("readFailed").d("reason", "invalidNumWords").d("numWords", nWords));
        return Error::INVALID;
    }

    // Check if closed.
    auto readerCloseIndex = m_readerCloseIndex->load();
    if (*m_readerCursor >= readerCloseIndex) {
        return Error::CLOSED;
    }

    // Initial check for overrun.
    auto header = m_bufferLayout->getHeader();
    if ((header->writeEndCursor >= *m_readerCursor) &&
        (header->writeEndCursor - *m_readerCursor) > m_bufferLayout->getDataSize()) {
        return Error::OVERRUN;
    }

    std::unique_lock<std::mutex> lock(header->dataAvailableMutex, std::defer_lock);
    if (Policy::BLOCKING == m_policy) {
        lock.lock();
    }

    // Figure out how much we can actually copy in a given Circular @c Buffer.
    size_t wordsAvailable = tell(Reference::BEFORE_WRITER);
    if (0 == wordsAvailable) {
		// Verify that the @c Writer is close.
        if (header->writeEndCursor > 0 && !header->isWriterEnabled) {
            return Error::CLOSED;
        } else if (Policy::NONBLOCKING == m_policy) {
            return Error::WOULDBLOCK;
        } else if (Policy::BLOCKING == m_policy) {
            // Condition for returning from read: the Writer has been closed or there is data to read
            auto predicate = [this, header] {
                return header->hasWriterBeenClosed || tell(Reference::BEFORE_WRITER) > 0;
            };

            if (std::chrono::milliseconds::zero() == timeout) {
                header->dataAvailableConditionVariable.wait(lock, predicate);
            } else if (!header->dataAvailableConditionVariable.wait_for(lock, timeout, predicate)) {
                return Error::TIMEDOUT;
            }
        }
        wordsAvailable = tell(Reference::BEFORE_WRITER);

        // If there is still no data, the writer has closed in the interim
        if (0 == wordsAvailable) {
            return Error::CLOSED;
        }
    }

    if (Policy::BLOCKING == m_policy) {
        lock.unlock();
    }
    if (nWords > wordsAvailable) {
        nWords = wordsAvailable;
    }

    // Don't read beyond our close index.
    if ((*m_readerCursor + nWords) > readerCloseIndex) {
        nWords = readerCloseIndex - *m_readerCursor;
    }

    // Split it across the wrap.
    size_t beforeWrap = m_bufferLayout->wordsUntilWrap(*m_readerCursor);
    if (beforeWrap > nWords) {
        beforeWrap = nWords;
    }
    size_t afterWrap = nWords - beforeWrap;

    // Copy the two segments. We need to consider the left and right sides of the current @c Writer's cursor.
    auto buf8 = static_cast<uint8_t*>(buf);
    memcpy(buf8, m_bufferLayout->getData(*m_readerCursor), beforeWrap * getWordSize());
    if (afterWrap > 0) {
        memcpy(
            buf8 + (beforeWrap * getWordSize()),
            m_bufferLayout->getData(*m_readerCursor + beforeWrap),
            afterWrap * getWordSize());
    }

    // Advance the read cursor.
    *m_readerCursor += nWords;

    // Final check for overrun (do this before the updateOldestUnconsumedCursor() call below for improved accuracy).
    // for writer BLOCKING.
    bool overrun = ((header->writeEndCursor - *m_readerCursor) > m_bufferLayout->getDataSize());

    // Now we can safely error out if there was an overrun.
    if (overrun) {
        return Error::OVERRUN;
    }

    return nWords;
}

bool Reader::seek(BufferLayout::Index offset, Reference reference) {
    auto header = m_bufferLayout->getHeader();
    auto writeStartCursor = &header->writeStartCursor;
    auto writeEndCursor = &header->writeEndCursor;
    BufferLayout::Index absolute = std::numeric_limits<BufferLayout::Index>::max();

    switch (reference) {
        case Reference::AFTER_READER:
            absolute = *m_readerCursor + offset;
            break;
        case Reference::BEFORE_READER:
            if (offset > *m_readerCursor) {
                AISDK_ERROR(LX("seekFailed")
                               .d("reason", "seekBeforeStreamStart")
                               .d("reference", "BEFORE_READER")
                               .d("seekOffset", offset)
                               .d("readerCursor", m_readerCursor->load()));
                return false;
            }
            absolute = *m_readerCursor - offset;
            break;
        case Reference::BEFORE_WRITER:
            if (offset > *writeStartCursor) {
                AISDK_ERROR(LX("seekFailed")
                               .d("reason", "seekBeforeStreamStart")
                               .d("reference", "BEFORE_WRITER")
                               .d("seekOffset", offset)
                               .d("writeStartCursor", writeStartCursor->load()));
                return false;
            }
            absolute = *writeStartCursor - offset;
            break;
        case Reference::ABSOLUTE:
            absolute = offset;
    }

    // Don't seek beyond the close index.
    if (absolute > *m_readerCloseIndex) {
        AISDK_ERROR(LX("seekFailed")
                       .d("reason", "seekBeyondCloseIndex")
                       .d("position", absolute)
                       .d("readerCloseIndex", m_readerCloseIndex->load()));
        return false;
    }

    bool backward = absolute < *m_readerCursor;
    std::unique_lock<std::mutex> lock(header->backwardSeekMutex, std::defer_lock);
    if (backward) {
        lock.lock();
    }

    // Don't seek to past data which has been (or soon will be) overwritten.
    if (*writeEndCursor >= absolute && *writeEndCursor - absolute > m_bufferLayout->getDataSize()) {
        AISDK_ERROR(LX("seekFailed").d("reason", "seekOverwrittenData"));
        return false;
    }

    *m_readerCursor = absolute;

    if (backward) {
        lock.unlock();
    }

    return true;
}

BufferLayout::Index Reader::tell(Reference reference) const {
    auto writeStartCursor = &m_bufferLayout->getHeader()->writeStartCursor;
    switch (reference) {
		/// Unknow
        case Reference::AFTER_READER:
            return 0;
		/// Unknow
        case Reference::BEFORE_READER:
            return 0;
		/// Get vaild unconsumed data.
        case Reference::BEFORE_WRITER:
            return (*writeStartCursor >= *m_readerCursor) ? *writeStartCursor - *m_readerCursor : 0;
		/// Get current reader cursor.
        case Reference::ABSOLUTE:
            return *m_readerCursor;
    }
    AISDK_ERROR(LX("tellFailed").d("reason", "invalidReference"));
    return std::numeric_limits<BufferLayout::Index>::max();
}

void Reader::close(BufferLayout::Index offset, Reference reference) {
    auto writeStartCursor = &m_bufferLayout->getHeader()->writeStartCursor;
    BufferLayout::Index absolute = 0;
    bool validReference = false;
    switch (reference) {
        case Reference::AFTER_READER:
            absolute = *m_readerCursor + offset;
            validReference = true;
            break;
        case Reference::BEFORE_READER:
            absolute = *m_readerCursor;
            validReference = true;
            break;
        case Reference::BEFORE_WRITER:
            if (*writeStartCursor < offset) {
                AISDK_ERROR(LX("closeFailed")
                               .d("reason", "invalidIndex")
                               .d("reference", "BEFORE_WRITER")
                               .d("offset", offset)
                               .d("writeStartCursor", writeStartCursor->load()));
            } else {
                absolute = *writeStartCursor - offset;
            }
            validReference = true;
            break;
        case Reference::ABSOLUTE:
            absolute = offset;
            validReference = true;
            break;
    }
    if (!validReference) {
        AISDK_ERROR(LX("closeFailed").d("reason", "invalidReference"));
    }

    *m_readerCloseIndex = absolute;
}

size_t Reader::getWordSize() const {
    return m_bufferLayout->getHeader()->wordSize;
}

size_t Reader::getId() const {
    return m_id;
}

#if 0
std::string Reader::errorToString(Error error) {
    switch (error) {
        case Error::CLOSED:
            return "CLOSED";
        case Error::OVERRUN:
            return "OVERRUN";
        case Error::WOULDBLOCK:
            return "WOULDBLOCK";
        case Error::TIMEDOUT:
            return "TIMEDOUT";
        case Error::INVALID:
            return "INVALID";
    }
    AISDK_ERROR(LX("errorToStringFailed").d("reason", "invalidError").d("error", error));
    return "(unknown error " + std::to_string(error) + ")";
}
#endif
}	// namespace sharedbuffer
}	//utils
} // namespace aisdk
