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
#include <Utils/DeviceInfo.h>
#include <KWD/KeywordDetectorRegister.h>

#include "Application/PortAudioMicrophoneWrapper.h"
#include "Application/AIClient.h"  //tmp
#include "Application/SampleApp.h"

static const std::string TAG{"SampleApp"};

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace application {

/// The sample rate of microphone audio data.
static const unsigned int SAMPLE_RATE_HZ = 16000;

/// The number of audio channels.
static const unsigned int NUM_CHANNELS = 1;

/// The size of each word within the stream.
static const size_t WORD_SIZE = 2;

/// The maximum number of readers of the stream.
static const size_t MAX_READERS = 4;

/// The amount of audio data to keep in the ring buffer.
static const std::chrono::seconds AMOUNT_OF_AUDIO_DATA_IN_BUFFER = std::chrono::seconds(15);

/// The size of the ring buffer.
static const size_t BUFFER_SIZE_IN_SAMPLES = (SAMPLE_RATE_HZ)*AMOUNT_OF_AUDIO_DATA_IN_BUFFER.count();

std::unique_ptr<SampleApp> SampleApp::createNew() {
	std::unique_ptr<SampleApp> instance(new SampleApp());
	if(!instance->initialize()){
		AISDK_ERROR(LX("createNewFailed").d("reason", "failed to initialize sampleApp"));
		return nullptr;
	}

	return instance;
}

void SampleApp::run() {
	/// We need an operation to listen for user input events or push-button events.
	/// To-Do Sven
	/// ...
	/// ...
	/// ...

	getchar();
}

SampleApp::~SampleApp() {
//	m_aiClient.reset();
	if(m_chatMediaPlayer) {
		m_chatMediaPlayer->shutdown();
	}
	if(m_streamMediaPlayer) {
		m_streamMediaPlayer->shutdown();
	}
	if(m_alarmMediaPlayer) {
		m_alarmMediaPlayer->shutdown();
	}
}

bool SampleApp::initialize() {
	AISDK_INFO(LX("initialize").d("reason", "Entry"));

	// Create a libao engine object.
	auto m_aoEngine = mediaPlayer::ffmpeg::AOEngine::create();
	if(!m_aoEngine) {
		AISDK_ERROR(LX("Failed to create media player engine!"));
		return false;
	}
	
	// Create a chatMediaPlayer of @c Pawrapper.
	m_chatMediaPlayer = mediaPlayer::ffmpeg::AOWrapper::create(m_aoEngine);
	if(!m_chatMediaPlayer) {
		AISDK_ERROR(LX("Failed to create media player for chat speech!"));
		return false;
	}

	m_streamMediaPlayer = mediaPlayer::ffmpeg::AOWrapper::create(m_aoEngine);
	if(!m_streamMediaPlayer) {
		AISDK_ERROR(LX("Failed to create media player for stream!"));
		return false;
	}

	m_alarmMediaPlayer = mediaPlayer::ffmpeg::AOWrapper::create(m_aoEngine);
	if(!m_alarmMediaPlayer) {
		AISDK_ERROR(LX("Failed to create media player for alarm!"));
		return false;
	}
	
	// To-Do Sven
	// To create other mediaplayer
	// ...

	// Creating the deviceInfo object
	std::string deviceInfoconfig("/tmp");
	std::shared_ptr<utils::DeviceInfo> deviceInfo = utils::DeviceInfo::create(deviceInfoconfig);
    if (!deviceInfo) {
        AISDK_ERROR(LX("Creation of DeviceInfo failed!"));
        return false;
    }

	// Creating UI manager
	auto userInterfaceManager = std::make_shared<UIManager>();

	// Create the AIClient to service those component.
	m_aiClient = aisdk::application::AIClient::createNew(
		deviceInfo,
		m_chatMediaPlayer,
		m_streamMediaPlayer,
		{userInterfaceManager},
		m_alarmMediaPlayer);
	if (!m_aiClient) {
        AISDK_ERROR(LX("Failed to create AI SDK client!"));
        return false;
    }
	
#if defined(KWD_IFLYTEK)
	// Step1.
	/*
     * Creating the buffer (Shared Buffer Stream) that will hold user audio data. This is the main input into the SDK.
     */
	size_t bufferSize = utils::sharedbuffer::SharedBuffer::calculateBufferSize(
		BUFFER_SIZE_IN_SAMPLES, WORD_SIZE, MAX_READERS);
	auto buffer = std::make_shared<utils::sharedbuffer::SharedBuffer::Buffer>(bufferSize);
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> sharedBufferStream = 
						utils::sharedbuffer::SharedBuffer::create(buffer, WORD_SIZE, MAX_READERS);
	if(!sharedBufferStream) {
		AISDK_ERROR(LX("Failed to create Shared buffer stream!"));
		return false;
	}

	// Step2.
	/// Creating capture audio data from microphone.
	std::shared_ptr<PortAudioMicrophoneWrapper> micWrapper = 
					PortAudioMicrophoneWrapper::create(sharedBufferStream);
	if(!micWrapper) {
		AISDK_ERROR(LX("Failed to create PortAudioMicrophone!"));
		return false;
	}

	// Step3.
	// Creating wake word audio provider, if a wake-up library already exists.
	// Currently only support SoundAi and IflyTek msc awake engine.
    // This observer is notified any time a keyword is detected and notifies the AIClient to start recognizing.
    auto keywordObserver =
        std::make_shared<KeywordObserver>(m_aiClient);

	m_keywordDetector = kwd::KeywordDetectorRegister::create(sharedBufferStream, {keywordObserver});
	if(!m_keywordDetector) {
		AISDK_ERROR(LX("Failed to create keyword detector!"));
		return false;
	}

	// Step4.
	// Creating the control action manager.
	m_controlActionManager = std::make_shared<ControlActionManager>(m_aiClient, micWrapper, userInterfaceManager);
#endif

	m_aiClient->connect();

	return true;
}


}  // namespace application
}  // namespace aisdk

