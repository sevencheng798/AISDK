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
#include "ASR/GenericAutomaticSpeechRecognizer.h"

namespace aisdk {
namespace asr {

using namespace utils::sharedbuffer;

/// String to identify log entries originating from this file.
static const std::string TAG("GenericAutomaticSpeechRecognizer");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) utils::logging::LogEntry(TAG, event)

void GenericAutomaticSpeechRecognizer::addASRObserver(
	std::shared_ptr<utils::soundai::SoundAiObserverInterface> asrObserver) {
	std::lock_guard<std::mutex> lock(m_asrObserversMutex);
	m_asrObservers.insert(asrObserver);
}

void GenericAutomaticSpeechRecognizer::removeASRObserver(
	std::shared_ptr<utils::soundai::SoundAiObserverInterface> asrObserver) {
	std::lock_guard<std::mutex> lock(m_asrObserversMutex);
	m_asrObservers.erase(asrObserver);
}

GenericAutomaticSpeechRecognizer::GenericAutomaticSpeechRecognizer(
    std::unordered_set<std::shared_ptr<utils::soundai::SoundAiObserverInterface>> asrObservers):
    SafeShutdown{"GenericAutomaticSpeechRecognizer"},
    m_asrObservers{asrObservers},
	m_state{utils::soundai::SoundAiObserverInterface::State::IDLE} {

}

ssize_t GenericAutomaticSpeechRecognizer::readFromStream(
    std::shared_ptr<utils::sharedbuffer::Reader> reader,
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
                                .d("reason", "streamOverrun"));
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

void GenericAutomaticSpeechRecognizer::setState(
	utils::soundai::SoundAiObserverInterface::State state) {
	std::lock_guard<std::mutex> lock(m_asrObserversMutex);
    if (m_state == state) {
        return;
    }

    AISDK_DEBUG(LX("setState").d("from", m_state).d("to", state));
    m_state = state;
    for (auto observer : m_asrObservers) {
        observer->onStateChanged(m_state);
    }
}

utils::soundai::SoundAiObserverInterface::State GenericAutomaticSpeechRecognizer::getState() const {
	return m_state;
}

void GenericAutomaticSpeechRecognizer::doShutdown() {
	m_asrObservers.clear();
	terminate();
}

}  // namespace asr
}  // namespace aisdk
