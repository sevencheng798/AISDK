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

#ifndef __APPLICATION_AICLIENT_H_
#define __APPLICATION_AICLIENT_H_

#include <unordered_set>
#include <vector>

#include <Utils/DeviceInfo.h>
#include <Utils/MediaPlayer/MediaPlayerInterface.h>
#include <Utils/DialogRelay/DialogUXStateObserverInterface.h>
#include <Utils/DialogRelay/DialogUXStateRelay.h>
/// Stream buffer .
#include <Utils/SharedBuffer/SharedBuffer.h>

#include <DMInterface/DomainSequencerInterface.h>

#include "AudioTrackManager/AudioTrackManager.h"
#ifdef ENABLE_SOUNDAI_ASR
#include "SoundAi/SoundAiEngine.h"
#elif ENABLE_IFLYTEK_AIUI_ASR
#include <ASR/GenericAutomaticSpeechRecognizer.h>
#endif
#include "SpeechSynthesizer/SpeechSynthesizer.h"

namespace aisdk {
namespace application {

/**
 * This class is used to instantiate each interactive component
 */
class AIClient {
public:
	/**
	 * Creates and initializes a default AI client. To connect the client to soundai server, users should make a call to
	 * connect() after creation.
	 */
	static std::unique_ptr<AIClient> createNew(
		std::shared_ptr<utils::DeviceInfo> deviceInfo,
		std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> chatMediaPlayer,
		std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> streamMediaPlayer,
		std::unordered_set<std::shared_ptr<utils::dialogRelay::DialogUXStateObserverInterface>>
        	dialogStateObservers,
        std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> alarmMediaPlayer);

	bool notifyOfWakeWord(
		std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
		utils::sharedbuffer::SharedBuffer::Index beginIndex,
		utils::sharedbuffer::SharedBuffer::Index endIndex);

	/**
	 * Start service to allow the client to initiate connect sai sdk service. 
	 */
	void connect();

	/**
	 * Stop service to allow the client to initiate disconnect sai sdk.
	 */
	void disconnect();
	
    /**
     * Destructor.
     */
    ~AIClient();

private:
    /**
     * Initializes the SDK and "glues" all the components together.
     */
	bool initialize(
		std::shared_ptr<utils::DeviceInfo> deviceInfo,
		std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> chatMediaPlayer,
		std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> streamMediaPlayer,
		std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> alarmMediaPlayer,
		std::unordered_set<std::shared_ptr<utils::dialogRelay::DialogUXStateObserverInterface>>
			dialogStateObservers);


	/// The AI dialog UX State.
	std::shared_ptr<utils::dialogRelay::DialogUXStateRelay> m_dialogUXStateRelay;

	/// The domain directive sequence.
	std::shared_ptr<dmInterface::DomainSequencerInterface> m_domainSequencer;

	/// The SoundAi client engine.
#ifdef ENABLE_SOUNDAI_ASR
	std::shared_ptr<soundai::engine::SoundAiEngine> m_soundAiEngine;
#elif ENABLE_IFLYTEK_AIUI_ASR
	std::shared_ptr<asr::GenericAutomaticSpeechRecognizer> m_asrEngine;
#endif

	/// The AudioTrack manager for audio channel player.
	std::shared_ptr<atm::AudioTrackManager> m_audioTrackManager;

	/// The speech synthesizer.
	std::shared_ptr<domain::speechSynthesizer::SpeechSynthesizer> m_speechSynthesizer;
};

}  // namespace application
}  // namespace aisdk

#endif  // __APPLICATION_AICLIENT_H_

