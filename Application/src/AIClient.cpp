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
#include <Utils/Attachment/AttachmentManager.h>
#include <NLP/DomainSequencer.h>
#include <NLP/MessageInterpreter.h>
#include <ASR/MessageConsumer.h>

#ifdef ENABLE_SOUNDAI_ASR
#include <SoundAi/MessageConsumer.h>
#include <KeywordDetector/KeywordDetector.h>
#endif

#include <ASR/AutomaticSpeechRecognizerRegister.h>
#include "Application/AIClient.h"

/// String to identify log entries originating from this file.
static const std::string TAG{"AIClient"};
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace application {

std::unique_ptr<AIClient> AIClient::createNew(
	std::shared_ptr<utils::DeviceInfo> deviceInfo,
	std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> chatMediaPlayer,
	std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> streamMediaPlayer,
	std::unordered_set<std::shared_ptr<utils::dialogRelay::DialogUXStateObserverInterface>>
    	dialogStateObservers,
  	std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> alarmMediaPlayer) {
    std::unique_ptr<AIClient> aiClient(new AIClient());
	if(!aiClient->initialize(
		deviceInfo,
		chatMediaPlayer,
		streamMediaPlayer,
		alarmMediaPlayer,
		dialogStateObservers
	)) {
		return nullptr;
	}

	AISDK_DEBUG0(LX("CreateSuccess"));
	
	return aiClient;
}

#if 0
std::unique_ptr<AIClient> AIClient::createNew() {
	return std::unique_ptr<AIClient>(new AIClient()); 
}
#endif

bool AIClient::initialize(
	std::shared_ptr<utils::DeviceInfo> deviceInfo,
	std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> chatMediaPlayer,
	std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> streamMediaPlayer,
	std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> alarmMediaPlayer,
	std::unordered_set<std::shared_ptr<utils::dialogRelay::DialogUXStateObserverInterface>>
    	dialogStateObservers) {
	if(!deviceInfo){
		AISDK_ERROR(LX("initializeFailed").d("reason", "nulldeviceInfo"));
		return false;
	}

	if(!chatMediaPlayer){
		AISDK_ERROR(LX("initializeFailed").d("reason", "nullChatMediaPlayer"));
		return false;
	}

	if(!streamMediaPlayer) {
		AISDK_ERROR(LX("initializeFailed").d("reason", "nullStreamMediaPlayer"));
		return false;
	}

	if(!alarmMediaPlayer) {
		AISDK_ERROR(LX("initializeFailed").d("reason", "nullAlarmMediaPlayer"));
		return false;
	}

	m_dialogUXStateRelay = std::make_shared<utils::dialogRelay::DialogUXStateRelay>();

    for (auto observer : dialogStateObservers) {
        m_dialogUXStateRelay->addObserver(observer);
    }

	/*
	* Creating the Domain directive Sequencer - This is the component that deals with the sequencing and ordering of
	* directives sent from SAI SDK and forwarding them along to the appropriate Domain Proxy that deals with
	* directives in that domain name.
	*/

	/*To-Do implement domainSequencer method*/
	m_domainSequencer = nlp::DomainSequencer::create();
	if(!m_domainSequencer) {
		AISDK_ERROR(LX("initializeFailed").d("reason", "unableToCreateDomainSequencer"));
		return false;
	}
	/// Creating the @c AttachmentManager.
	auto attachmentDocker = std::make_shared<utils::attachment::AttachmentManager>();
	
	/// Creating the domain messageinterpreter.
	auto messageInterpreter = std::make_shared<nlp::MessageInterpreter>(m_domainSequencer, attachmentDocker);

	/// Creating a message consumer
	auto messageConsumer = std::make_shared<asr::MessageConsumer>(messageInterpreter);
#ifdef ENABLE_SOUNDAI_ASR

	/// Creating soundai client engine.
	
	const std::string soundAiConfigPath("/cfg/sai_config");
	m_soundAiEngine = soundai::engine::SoundAiEngine::create(deviceInfo, messageConsumer, soundAiConfigPath);
	if(!m_soundAiEngine) {
		AISDK_ERROR(LX("initializeFailed").d("reason", "unableToCreateSoundAiEngine"));
		return false;
	}
#endif	
    /*
     * Creating the Audio Track Manager
     */
    m_audioTrackManager = std::make_shared<atm::AudioTrackManager>();

#ifdef ENABLE_SOUNDAI_ASR

	/*
	 * Creating the keyword observer - This is the commponent that deals with listener the soundai wakeup state.
	 */
	auto keywordObserver = std::make_shared<aisdk::kwd::KeywordDetector>(m_audioTrackManager);

	// Add ai observer to ai engine.
	m_soundAiEngine->addObserver(keywordObserver);
	m_soundAiEngine->addObserver(m_dialogUXStateRelay);
#endif

#ifdef ENABLE_IFLYTEK_AIUI_ASR
	
	if(!attachmentDocker) {
		AISDK_ERROR(LX("initializeFailed").d("reason", "unableToCreateAttachmentDocker"));
		return false;
	}
	
	m_asrEngine = asr::AutomaticSpeechRecognizerRegister::create(m_audioTrackManager, attachmentDocker, messageConsumer);
	if(!m_asrEngine) {
		AISDK_ERROR(LX("initializeFailed").d("reason", "unableToCreateASREngine"));
		return false;
	}

	m_asrEngine->addASRObserver(m_dialogUXStateRelay);

#endif
	/*
	 * Creating the speech synthesizer. This is the commponent that deals with real-time interactive domain.
	 */
	m_speechSynthesizer = domain::speechSynthesizer::SpeechSynthesizer::create(
		chatMediaPlayer,
		m_audioTrackManager,
		m_dialogUXStateRelay);
	if (!m_speechSynthesizer) {
        AISDK_ERROR(LX("initializeFailed").d("reason", "unableToCreateSpeechSynthesizer"));
        return false;
    }

	m_speechSynthesizer->addObserver(m_dialogUXStateRelay);

	/// To-Do Sven
	/// Continue to add other domain commponent.
	/// ...
	/// ...
	/// ...

	
	/*
	 * The following statements show how to register domain relay commponent to the domain directive sequencer.
	 */
	if (!m_domainSequencer->addDomainHandler(m_speechSynthesizer)) {
		AISDK_ERROR(LX("initializeFailed")
						.d("reason", "unableToRegisterDomainHandler")
						.d("domainHandler", "SpeechSynthesizer"));
		return false;
	}

	/// To-Do Sven
	/// Continue to add other domain commponent.
	/// ...
	/// ...
	/// ...

	return true;
}

bool AIClient::notifyOfWakeWord(
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
	utils::sharedbuffer::SharedBuffer::Index beginIndex,
	utils::sharedbuffer::SharedBuffer::Index endIndex) {
	AISDK_DEBUG5(LX("notifyOfWakeWord").d("wakeup", "COMING"));

	m_asrEngine->recognize(stream, beginIndex, endIndex);
	return true;
}

void AIClient::connect() {
#ifdef ENABLE_SOUNDAI_ASR	
	m_soundAiEngine->start();
#endif
}

void AIClient::disconnect() {
#ifdef ENABLE_SOUNDAI_ASR
	m_soundAiEngine->stop();
#endif	
}

AIClient::~AIClient() {
	if(m_domainSequencer) {
		AISDK_DEBUG5(LX("DomainSequencerShutdown"));
        m_domainSequencer->shutdown();
	}

	if(m_speechSynthesizer) {
		AISDK_DEBUG5(LX("SpeechSynthesizerShutdown"));
		m_speechSynthesizer->shutdown();
	}
}

}  // namespace application
}  // namespace aisdk

