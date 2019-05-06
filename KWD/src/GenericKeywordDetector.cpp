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

#include <Utils/Logging/Logger.h>

#include "KWD/GenericKeywordDetector.h"

namespace aisdk {
namespace kwd {

using namespace utils::sharedbuffer;
using namespace dmInterface;

/// String to identify log entries originating from this file.
static const std::string TAG("GenericKeywordDetector");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) utils::logging::LogEntry(TAG, event)

// TEST
void GenericKeywordDetector::addKeyWordObserver(std::shared_ptr<KeyWordObserverInterface> keyWordObserver) {
    std::lock_guard<std::mutex> lock(m_keyWordObserversMutex);
    m_keyWordObservers.insert(keyWordObserver);
}
//TEST
void GenericKeywordDetector::removeKeyWordObserver(std::shared_ptr<KeyWordObserverInterface> keyWordObserver) {
    std::lock_guard<std::mutex> lock(m_keyWordObserversMutex);
    m_keyWordObservers.erase(keyWordObserver);
}

GenericKeywordDetector::GenericKeywordDetector(
    std::unordered_set<std::shared_ptr<KeyWordObserverInterface>> keyWordObservers) :
        m_keyWordObservers{keyWordObservers} {
}

void GenericKeywordDetector::notifyKeyWordObservers(
    std::shared_ptr<SharedBuffer> stream,
    std::string keyword,
    SharedBuffer::Index beginIndex,
    SharedBuffer::Index endIndex) const {
    std::lock_guard<std::mutex> lock(m_keyWordObserversMutex);
    for (auto keyWordObserver : m_keyWordObservers) {
        keyWordObserver->onKeyWordDetected(stream, keyword);
    }
}

ssize_t GenericKeywordDetector::readFromStream(
    std::shared_ptr<utils::sharedbuffer::Reader> reader,
    std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
    void* buf,
    size_t nWords,
    std::chrono::milliseconds timeout,
    bool* errorOccurred) {
    if (errorOccurred) {
        *errorOccurred = false;
    }
    ssize_t wordsRead = reader->read(buf, nWords, timeout);
    // Stream has been closed
    if (wordsRead == 0) {
        AISDK_DEBUG(LX("readFromStream").d("event", "streamClosed"));
        if (errorOccurred) {
            *errorOccurred = true;
        }
        // This represents some sort of error with the read() call
    } else if (wordsRead < 0) {
        switch (wordsRead) {
            case Reader::Error::OVERRUN:
                AISDK_ERROR(LX("readFromStreamFailed")
                                .d("reason", "streamOverrun")
                                .d("numWordsOverrun",
                                   std::to_string(
                                       reader->tell(Reader::Reference::BEFORE_WRITER) -
                                       stream->getDataSize())));
				/**
				 * Synchronously readerCursor to the current writer cursor position.
				 * Preparing for the next reading.
				 */
                reader->seek(0, Reader::Reference::BEFORE_WRITER);
                break;
            case Reader::Error::TIMEDOUT:
                AISDK_INFO(LX("readFromStreamFailed").d("reason", "readerTimeOut"));
                break;
            default:
                // We should never get this since we are using a Blocking Reader.
                AISDK_ERROR(LX("readFromStreamFailed")
                                .d("reason", "unexpectedError")
                                // Leave as ssize_t to avoid messiness of casting to enum.
                                .d("error", wordsRead));

                if (errorOccurred) {
                    *errorOccurred = true;
                }
                break;
        }
    }
    return wordsRead;
}

bool GenericKeywordDetector::isByteswappingRequired() {
    bool isPlatformLittleEndian = false;
    int num = 1;
    char* firstBytePtr = reinterpret_cast<char*>(&num);
    if (*firstBytePtr == 1) {
        isPlatformLittleEndian = true;
    }

    bool isFormatLittleEndian = true;
    return isPlatformLittleEndian != isFormatLittleEndian;
}

}  // namespace kwd
}  // namespace aisdk
