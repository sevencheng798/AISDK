/*
 * Copyright 2019 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include "Utils/Attachment/InProcessAttachmentReader.h"
#include "Utils/Logging/Logger.h"

using namespace aisdk::utils;

namespace aisdk {
namespace utils {
namespace attachment {

/// String to identify log entries originating from this file.
static const std::string TAG("InProcessAttachmentReader");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

std::unique_ptr<InProcessAttachmentReader> InProcessAttachmentReader::create(
    SDSTypeReader::Policy policy,
    std::shared_ptr<SDSType> sds,
    SDSTypeIndex offset,
    SDSTypeReader::Reference reference) {
    auto reader = std::unique_ptr<InProcessAttachmentReader>(new InProcessAttachmentReader(policy, sds));

    if (!reader->m_reader) {
        AISDK_ERROR(LX("createFailed").d("reason", "object not fully created"));
        return nullptr;
    }

    if (!reader->m_reader->seek(offset, reference)) {
        AISDK_ERROR(LX("ConstructorFailed").d("reason", "seek failed"));
        return nullptr;
    }

    return reader;
}

InProcessAttachmentReader::InProcessAttachmentReader(SDSTypeReader::Policy policy, std::shared_ptr<SDSType> sds) {
    if (!sds) {
        AISDK_ERROR(LX("ConstructorFailed").d("reason", "SDS parameter is nullptr"));
        return;
    }

    m_reader = sds->createReader(policy);

    if (!m_reader) {
        AISDK_ERROR(LX("ConstructorFailed").d("reason", "could not create an SDS reader"));
        return;
    }
}

InProcessAttachmentReader::~InProcessAttachmentReader() {
    close();
}

std::size_t InProcessAttachmentReader::read(
    void* buf,
    std::size_t numBytes,
    ReadStatus* readStatus,
    std::chrono::milliseconds timeoutMs) {
    if (!readStatus) {
        AISDK_ERROR(LX("readFailed").d("reason", "read status is nullptr"));
        return 0;
    }

    if (!m_reader) {
        AISDK_INFO(LX("readFailed").d("reason", "closed or uninitialized SDS"));
        *readStatus = ReadStatus::CLOSED;
        return 0;
    }

    if (timeoutMs.count() < 0) {
        AISDK_ERROR(LX("readFailed").d("reason", "negative timeout"));
        *readStatus = ReadStatus::ERROR_INTERNAL;
        return 0;
    }

    *readStatus = ReadStatus::OK;

    if (0 == numBytes) {
        return 0;
    }

    auto wordSize = m_reader->getWordSize();
    if (numBytes < wordSize) {
        AISDK_ERROR(LX("readFailed").d("reason", "bytes requested smaller than SDS word size"));
        *readStatus = ReadStatus::ERROR_BYTES_LESS_THAN_WORD_SIZE;
        return 0;
    }

    std::size_t bytesRead = 0;
    auto numWords = numBytes / wordSize;

    auto readResult = m_reader->read(buf, numWords, timeoutMs);

    /*
     * Convert SDS return code accordingly:
     *
     * < 0 : Error code.
     *   0 : The underlying SDS is closed.
     * > 0 : The number of bytes read.
     */

    if (readResult < 0) {
        switch (readResult) {
            // This means the writer has overwritten the reader.  An attachment cannot recover from this.
            case SDSTypeReader::Error::OVERRUN:
                *readStatus = ReadStatus::ERROR_OVERRUN;
                AISDK_ERROR(LX("readFailed").d("reason", "memory overrun by writer"));
                close();
                break;

            // This means there is still an active writer, but no data.  A read would block if the policy was blocking.
            case SDSTypeReader::Error::WOULDBLOCK:
                *readStatus = ReadStatus::OK_WOULDBLOCK;
                break;

            // This means there is still an active writer, but no data.  A read call timed out waiting for data.
            case SDSTypeReader::Error::TIMEDOUT:
                *readStatus = ReadStatus::OK_TIMEDOUT;
                break;
        }

        // If the status was not updated, then there's an error code from SDS we may not be handling.
        if (ReadStatus::OK == *readStatus) {
            AISDK_ERROR(LX("readFailed").d("reason", "unhandled error code").d("code", readResult));
            *readStatus = ReadStatus::ERROR_INTERNAL;
        }

    } else if (0 == readResult) {
        *readStatus = ReadStatus::CLOSED;
        AISDK_DEBUG0(LX("readFailed").d("reason", "SDS is closed"));
    } else {
        bytesRead = static_cast<size_t>(readResult) * wordSize;
    }

    return bytesRead;
}

void InProcessAttachmentReader::close(ClosePoint closePoint) {
    if (m_reader) {
        switch (closePoint) {
            case ClosePoint::IMMEDIATELY:
                m_reader->close();
                return;
            case ClosePoint::AFTER_DRAINING_CURRENT_BUFFER:
                m_reader->close(0, SDSTypeReader::Reference::BEFORE_WRITER);
                return;
        }
    }
}

bool InProcessAttachmentReader::seek(uint64_t offset) {
    if (m_reader) {
        return m_reader->seek(offset);
    }
    return false;
}

uint64_t InProcessAttachmentReader::getNumUnreadBytes() {
    if (m_reader) {
        return m_reader->tell(SDSTypeReader::Reference::BEFORE_WRITER);
    }

    AISDK_ERROR(LX("getNumUnreadBytesFailed").d("reason", "noReader"));
    return 0;
}

}  // namespace attachment
}  // namespace utils
}  // namespace aisdk
