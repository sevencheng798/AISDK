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

#ifndef __AUTOMATIC_SPEECH_RECOGNIZER_CONFIGURATION_H_
#define __AUTOMATIC_SPEECH_RECOGNIZER_CONFIGURATION_H_

#include <mutex>
#include <unordered_set>

#include <Utils/SharedBuffer/SharedBuffer.h>
#include <Utils/SoundAi/SoundAiObserverInterface.h>

namespace aisdk {
namespace asr {
	
// Create the automatic speech recognizer configuration object with explicit configuration values for all fields.
class AutomaticSpeechRecognizerConfiguration {
public:
	inline std::string getSoundAiConfigPath() const;

	inline std::string getAiuiAppId() const;

	inline std::string getAiuiConfigFile() const;
	
	inline std::string getAiuiDir() const;

	inline std::string getAiuiLogDir() const;
	
    /**
     * Configurable constructor that can be used to set soundai configuration values.
     */
	AutomaticSpeechRecognizerConfiguration(
		const std::string &configPath):
		m_soundAiConfigPath{configPath} {

	};
    /**
     * Configurable constructor that can be used to set aiui configuration values.
     */
	AutomaticSpeechRecognizerConfiguration(
		const std::string &appId = "5c3d4427",
		const std::string &aiuiConfigFile = "/data/soundai/AIUI/cfg/aiui.cfg",
		const std::string &aiuiDir = "/data/soundai/AIUI/",
		const std::string &aiuiLogDir = "/data/soundai/AIUI/log/"):
		m_aiuiAppId{appId},
		m_aiuiConfigFile{aiuiConfigFile},
		m_aiuiDir{aiuiDir},
		m_aiuiLogDir{aiuiLogDir} {

	};
    /**
     * Destructor.
     */
    ~AutomaticSpeechRecognizerConfiguration() = default;

private:

	const std::string m_soundAiConfigPath;

	const std::string m_aiuiAppId;

	const std::string m_aiuiConfigFile;
	
	const std::string m_aiuiDir;

	const std::string m_aiuiLogDir;

};

std::string AutomaticSpeechRecognizerConfiguration::getSoundAiConfigPath() const {
	return m_soundAiConfigPath;
}

std::string AutomaticSpeechRecognizerConfiguration::getAiuiAppId() const {
	return m_aiuiAppId;
}

std::string AutomaticSpeechRecognizerConfiguration::getAiuiConfigFile() const {
	return m_aiuiConfigFile;
}

std::string AutomaticSpeechRecognizerConfiguration::getAiuiDir() const {
	return m_aiuiDir;
}

std::string AutomaticSpeechRecognizerConfiguration::getAiuiLogDir() const {
	return m_aiuiLogDir;
}

}  // namespace asr
}  // namespace aisdk

#endif  // __AUTOMATIC_SPEECH_RECOGNIZER_CONFIGURATION_H_