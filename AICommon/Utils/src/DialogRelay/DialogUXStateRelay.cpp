/*
 * Copyright 2019 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
 
#include <iostream>
#include <unistd.h>

#include "Utils/Logging/Logger.h"
#include "Utils/DialogRelay/DialogUXStateRelay.h"

static const std::string TAG{"DialogUXStateRelay"};
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace utils {
namespace dialogRelay {

DialogUXStateRelay::DialogUXStateRelay()
	:m_currentState{DialogUXStateObserverInterface::DialogUXState::IDLE},
	m_soundAiState{soundai::SoundAiObserverInterface::State::IDLE},
	m_speechSynthesizerState{dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED} {

}

void DialogUXStateRelay::addObserver(
	std::shared_ptr<DialogUXStateObserverInterface> observer) {
    if (!observer) {
        AISDK_ERROR(LX("addObserverFailed").d("reason", "nullObserver"));
        return;
    }
	
    m_executor.submit([this, observer]() {
        m_observers.insert(observer);
        observer->onDialogUXStateChanged(m_currentState);
    });
}

void DialogUXStateRelay::removeObserver(
	std::shared_ptr<DialogUXStateObserverInterface> observer) {
    if (!observer) {
        AISDK_ERROR(LX("removeObserverFailed").d("reason", "nullObserver"));
        return;
    }
    m_executor.submit([this, observer]() { m_observers.erase(observer); }).wait();
}

void DialogUXStateRelay::onStateChanged(soundai::SoundAiObserverInterface::State state){
	m_soundAiState = state;
	
	// AISDK_INFO(LX("onStateChanged").d("SoundAiNewState", state));
	
	m_executor.submit([this, state](){
		switch(state){
			case soundai::SoundAiObserverInterface::State::IDLE:
//				setState(dialogRelay::DialogUXStateObserverInterface::DialogUXState::IDLE);
				tryEnterIdleState();
				return;
			case soundai::SoundAiObserverInterface::State::EXPECTING_SPEECH:
				setState(dialogRelay::DialogUXStateObserverInterface::DialogUXState::LISTENING);
				return;
			case soundai::SoundAiObserverInterface::State::RECOGNIZING:
				setState(dialogRelay::DialogUXStateObserverInterface::DialogUXState::LISTENING);
				return;
			case soundai::SoundAiObserverInterface::State::BUSY:
				setState(dialogRelay::DialogUXStateObserverInterface::DialogUXState::THINKING);
				return;
		}
		
		AISDK_WARN(LX("unknownSoundAiProcessorState"));
	});
}

void DialogUXStateRelay::onKeyWordDetected(std::string dialogId, std::string keyword, float angle){
	/// default no-op
}

void DialogUXStateRelay::onStateChanged(dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState state) {
	// AISDK_INFO(LX("onStateChanged").d("SpeechSynth", state));
    m_speechSynthesizerState = state;

    m_executor.submit([this, state]() {
        switch (state) {
            case dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState::PLAYING:
                setState(DialogUXStateObserverInterface::DialogUXState::SPEAKING);
                return;
            case dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED:
                 m_executor.submit([this]() {
			        if (m_currentState != DialogUXStateObserverInterface::DialogUXState::IDLE &&
			            m_soundAiState == soundai::SoundAiObserverInterface::State::IDLE &&
			            m_speechSynthesizerState == dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED) {
			            setState(DialogUXStateObserverInterface::DialogUXState::IDLE);
			        }
			    });
                return;
            case dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState::LOSING_FOCUS:
                return;
            case dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState::GAINING_FOCUS:
                return;
        }
		AISDK_WARN(LX("unknownSpeechSynthesizerState"));
    });

}

void DialogUXStateRelay::tryEnterIdleState() {
	m_executor.submit([this]() {
		// The delay ensures that ASR state is avoided from Thinking to IDLE.
		usleep(200*1000);
		if(m_currentState != dialogRelay::DialogUXStateObserverInterface::DialogUXState::IDLE && \
			m_speechSynthesizerState == dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState::FINISHED) {
			setState(dialogRelay::DialogUXStateObserverInterface::DialogUXState::IDLE);
		}
	});
}

void DialogUXStateRelay::notifyObserversOfState() {
    for (auto observer : m_observers) {
        if (observer) {
            observer->onDialogUXStateChanged(m_currentState);
        }
    }
}

void DialogUXStateRelay::setState(dialogRelay::DialogUXStateObserverInterface::DialogUXState newState){
	if(m_currentState == newState)
		return;

	AISDK_INFO(LX("setState").d("Dialog state from", m_currentState).d("to", newState));
	m_currentState = newState;
	notifyObserversOfState();
}

}	// namespace dialogRelay
}   // namespace utils
}   // namespace aisdk
