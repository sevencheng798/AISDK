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

#include <iostream>
#include <string>
#include <Utils/Logging/Logger.h>

#include "Application/UIManager.h"

static const std::string TAG{"UIManager"};
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace application {
	static const std::string HELP_MESSAGE =
		"+----------------------------------------------------------------------------+\n"
		"|									Options:								  |\n"
		"| Tap to talk: 															  |\n"
		"|		 Press 't' and Enter followed by your query (no need for the kewword  |\n"
		"|			'xiaokangxiaokang').											  |\n"
		"| Stop an interaction: 													  |\n"
		"|		 Press 's' and Enter to stop an ongoing interaction.				  |\n";

static const std::string IDLE_MESSAGE =
		"\n#############################################\n"
		"########## NLP Client is currently IDLE! ####\n"
		"#############################################\n";

static const std::string LISTEN_MESSAGE =
		"\n#############################################\n"
		"##########    LISTENING          ############\n"
		"#############################################\n";

static const std::string SPEAK_MESSAGE =
		"\n#############################################\n"
		"##########    SPEAKING          #############\n"
		"#############################################\n";

static const std::string THINK_MESSAGE =
		"\n#############################################\n"
		"##########    THINKING          #############\n"
		"#############################################\n";

UIManager::UIManager():
	m_dialogState{DialogUXStateObserverInterface::DialogUXState::IDLE} {

}

void UIManager::onDialogUXStateChanged(DialogUXStateObserverInterface::DialogUXState newState) {
	m_executor.submit([this, newState]() {
			if(m_dialogState == newState)
				return;

			m_dialogState = newState;
			printState();
		});
}

void UIManager::printErrorScreen() {
    m_executor.submit([]() { AISDK_INFO(LX("Invalid Option")); });
}

void UIManager::printHelpScreen() {
    m_executor.submit([]() { AISDK_INFO(LX(HELP_MESSAGE)); });
}

void UIManager::microphoneOff() {
    m_executor.submit([]() { AISDK_INFO(LX("Microphone Off!")); });
}

void UIManager::microphoneOn() {
    m_executor.submit([this]() { printState(); });
}

void UIManager::printState() {
	switch(m_dialogState) {
		case DialogUXStateObserverInterface::DialogUXState::IDLE:
			AISDK_INFO(LX(IDLE_MESSAGE));
			break;
		case DialogUXStateObserverInterface::DialogUXState::LISTENING:
			AISDK_INFO(LX(LISTEN_MESSAGE));
			break;
		case DialogUXStateObserverInterface::DialogUXState::THINKING:
			AISDK_INFO(LX(THINK_MESSAGE));
			break;
		case DialogUXStateObserverInterface::DialogUXState::SPEAKING:
			AISDK_INFO(LX(SPEAK_MESSAGE));
			break;
		case DialogUXStateObserverInterface::DialogUXState::FINISHED:
			break;
	}
}

} // namespace application
} // namespace aisdk
