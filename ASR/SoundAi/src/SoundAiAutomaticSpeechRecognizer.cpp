/*
 * Copyright 2018 gm its affiliates. All Rights Reserved.
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
#include <fstream>
#include <string>

#include <Utils/Logging/Logger.h>

#include "SoundAi/SoundAiAutomaticSpeechRecognizer.h"
#include "SoundAi/NetEventTypes.h"

/// String to identify log entries originating from this file.
static const std::string TAG{"SoundAiAutomaticSpeechRecognizer"};

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace asr {
namespace soundAiEngine {

/// The name of the @c AudioTrackManager channel used by the @c SpeechSynthesizer.
static const std::string CHANNEL_NAME = utils::channel::AudioTrackManagerInterface::DIALOG_CHANNEL_NAME;

/// The name of channel handler interface
static const std::string CHANNEL_INTERFACE_NAME{"SoundAiAutomaticSpeechRecognizer"};

const auto ASR_TIMEOUT = std::chrono::seconds{5};

/// Pointing to the class itself @c SoundAiEngine and pass to static function
SoundAiAutomaticSpeechRecognizer* SoundAiAutomaticSpeechRecognizer::m_soundAiEngine = nullptr;

std::shared_ptr<SoundAiAutomaticSpeechRecognizer> SoundAiAutomaticSpeechRecognizer::create(
		const std::shared_ptr<utils::DeviceInfo>& deviceInfo,
		std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager,
		std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker,
		std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
		const AutomaticSpeechRecognizerConfiguration& config) {

	if(!deviceInfo){
		AISDK_ERROR(LX("CreateFailed").d("reason", "deviceInfoIsNull"));
		return nullptr;
	}
	
	if(!trackManager) {
		AISDK_ERROR(LX("CreateFailed").d("reason", "trackManagerIsnullptr."));
		return nullptr;
	}

	if(!attachmentDocker) {
		AISDK_ERROR(LX("CreateFailed").d("reason", "attachmentDockerIsnullptr"));
	}
	
	if(!messageConsumer) {
		AISDK_ERROR(LX("CreateFailed").d("reason", "messageConsumerIsnullptr."));
		return nullptr;
	}

	auto configPath = config.getSoundAiConfigPath();
	auto threshold = config.getSoundAiThreshold();
	std::shared_ptr<SoundAiAutomaticSpeechRecognizer> instance(
		new SoundAiAutomaticSpeechRecognizer(
			deviceInfo, trackManager, attachmentDocker, messageConsumer, configPath, threshold));
	if(!instance->init()){
		AISDK_ERROR(LX("CreateFailed").d("reason", "initializeFailed"));
		return nullptr;
	}

	return instance;
}

bool SoundAiAutomaticSpeechRecognizer::start(){
	int ret = start_service();

	/// Set voip mode
	set_voip_flag(m_voipMode);
	
	return ((ret == 0)? true:false);
}

bool SoundAiAutomaticSpeechRecognizer::stop(){
	int ret = stop_service();

	return ((ret == 0)? true:false);
}

bool SoundAiAutomaticSpeechRecognizer::reset() {
	return true;
}

std::future<bool> SoundAiAutomaticSpeechRecognizer::recognize(
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
	utils::sharedbuffer::SharedBuffer::Index begin,
	utils::sharedbuffer::SharedBuffer::Index keywordEnd) {
	std::promise<bool> p;
    std::future<bool> ret = p.get_future();
	return ret;
}

std::future<bool> SoundAiAutomaticSpeechRecognizer::acquireTextToSpeech(
	std::string text,
	std::shared_ptr<utils::attachment::AttachmentWriter> writer) {
	std::promise<bool> p;
    std::future<bool> ret = p.get_future();
	return ret;

}

void SoundAiAutomaticSpeechRecognizer::onTrackChanged(utils::channel::FocusState newTrace) {

}

SoundAiAutomaticSpeechRecognizer::~SoundAiAutomaticSpeechRecognizer(){
	AISDK_INFO(LX("Destructor"));
	
	stop();
	terminate_system();
}

SoundAiAutomaticSpeechRecognizer::SoundAiAutomaticSpeechRecognizer(
	const std::shared_ptr<utils::DeviceInfo>& deviceInfo,
	std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager,
	std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker,
	std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
	const std::string &configPath,
	double threshold)
	:m_deviceInfo{deviceInfo}
	,m_trackManager{trackManager}
	,m_trackState{utils::channel::FocusState::NONE}
	,m_messageConsumer{messageConsumer}
	,m_config{configPath}
	,m_threshold{threshold}
	,m_voipMode{0}
	,m_logLevel{SAI_LOGGER_DEBUG} {
	m_soundAiEngine = this;

}

bool SoundAiAutomaticSpeechRecognizer::init(){
	std::string dsn = m_deviceInfo->getDeviceSerialNumber();
	
	/// Set logger print level
	set_logger_level(m_logLevel);
	
	/// Initialize data system
    int ret = init_system(dsn.c_str(), m_threshold, m_config.c_str(),
		this->asrDataCallback,		// maybe set null
		this->wakeupCallback, 
		this->ivwDataCallback, 		// maybe set null
		this->voipDataCallback, 	// maybe set null
		this->oneshotCallback, 
		this->wavEnergyCallback,	//null
		this->netEventCallback,
		this->netStatusCallback,
		this->beatDataCallback,
		reinterpret_cast<void *>(this), reinterpret_cast<void *>(this), reinterpret_cast<void *>(this));

	if(ret != 0) {
		AISDK_ERROR(LX("initializeFailed").d("reason", "init_system:ReturnError"));
		return false;
	}

	return true;
}

void SoundAiAutomaticSpeechRecognizer::asrDataCallback(void * usr_data_asr, const char * id, const char *buffer, size_t size){
	std::cout << __func__ << " :Entry" << std::endl;
	std::cout << "buffer: " << buffer << std::endl;
}

void SoundAiAutomaticSpeechRecognizer::notifyKeyWordObservers( 
	std::string dialogId,
	std::string keyword,
	float angle){
#if 0	
	std::lock_guard<std::mutex> lock{m_observerMutex};
	for (auto observer : m_observers) {
        if (observer) {
            observer->onKeyWordDetected(dialogId, keyword, angle);			
        }
    }
#endif	
}

int SoundAiAutomaticSpeechRecognizer::wakeupCallback(void * usr_data_wk, const char * id, const char * key_word,
						   float score, int wakeup_result, float angle, int usr_set){

   AISDK_INFO(LX("wakeupCallback")
   				.d("id", id)
   				.d("key_word", key_word)
   				.d("score", score)
   				.d("wakeup_result", wakeup_result)
   				.d("angle", angle)
   				.d("usr_set", usr_set));
   
	SoundAiAutomaticSpeechRecognizer* engine = static_cast<SoundAiAutomaticSpeechRecognizer*>(usr_data_wk);
	
	std::string dialogID = id;
	std::string keyword = key_word;

	/// To let sai sdk generate new dialog id
	if(dialogID.empty()){
		return 0;
	}
#if 0
	/// Notify observer to handler event
	engine->notifyKeyWordObservers(dialogID, keyword, angle);

	/// Comein capture stream state 
	engine->setState(SoundAiObserver::State::RECOGNIZING);
#endif

	engine->m_executor.submit([usr_data_wk, dialogID, keyword, angle]() {
	    SoundAiAutomaticSpeechRecognizer* engine = static_cast<SoundAiAutomaticSpeechRecognizer*>(usr_data_wk);
        return engine->executeRecognize(dialogID, keyword, angle);
    });

	/// Comein capture stream state 
//	engine->setState(SoundAiObserver::State::RECOGNIZING);
	
	return 0;
}

void SoundAiAutomaticSpeechRecognizer::ivwDataCallback(const char * id, const char *buffer, size_t size){
	std::cout << __func__ << " :Entry" << std::endl;	
	std::cout << "buffer: " << buffer << std::endl;
}

void SoundAiAutomaticSpeechRecognizer::voipDataCallback(void * usr_data_voip, const char* id, const char *buffer, size_t size){
	std::cout << __func__ << " :Entry" << std::endl;
	std::cout << "buffer: " << buffer << std::endl;
	// default no-op
}

void SoundAiAutomaticSpeechRecognizer::oneshotCallback(int event_type){
	std::cout << __func__ << " :Entry" << ": event_type: " << event_type << std::endl;
	// default no-op
}

void SoundAiAutomaticSpeechRecognizer::wavEnergyCallback(float val){
	//std::cout << __func__ << " :Entry" << std::endl;
	// default no-op
}

void SoundAiAutomaticSpeechRecognizer::netEventCallback(const char * id, int type, int error_code, const char* msg){
	AISDK_INFO(LX("netEventCallback")
			.d("id", id)
			.d("type", eventTypeToString(type))
			.d("code", codeInfoToString(error_code))
			.d(" msg", msg));
	
	if(type == SOUNDAI_VAD) {
		if(error_code == EVENT_VAD_END || error_code == EVENT_VAD_BEGIN_TIMEOUT) {
			set_unwakeup_status();

			/// Comein think state
			m_soundAiEngine->setState(SoundAiObserver::State::BUSY);
		}
	}else if(type == SOUNDAI_SEMANTICS){
		//m_soundAiEngine->setState(SoundAiObserver::State::IDLE);

		// To-Do
		// In here parse json data
		// ...
		// ...
		// ...
		m_soundAiEngine->m_messageConsumer->consumeMessage(id, msg);
		m_soundAiEngine->setState(SoundAiObserver::State::IDLE);
	}
	
}

void SoundAiAutomaticSpeechRecognizer::netStatusCallback(networkStatusCode code, const char* msg){
	AISDK_INFO(LX("netStatusCallback").d("code", code).d("msg", msg));

}

void SoundAiAutomaticSpeechRecognizer::beatDataCallback(const char * id, size_t len){
	//std::cout << __func__ << " :Entry" << std::endl;
	// default no-op
}

bool SoundAiAutomaticSpeechRecognizer::executeRecognize(
	std::string dialogId,
	std::string keyword,
	float angle) {

	// If this is a barge-in, verify that it is permitted.
	auto currentState = getState();
	switch(currentState) {
		case SoundAiObserver::State::IDLE:
			break;
		case SoundAiObserver::State::EXPECTING_SPEECH:
			break;
		case SoundAiObserver::State::RECOGNIZING:
			AISDK_ERROR(LX("executeRecognizeFailed").d("reason", "Barge-in is not permitted while recognizing"));
			return false;
		case SoundAiObserver::State::BUSY:
			AISDK_ERROR(LX("executeRecognizeFailed").d("reason", "Barge-in is not permitted while busy"));
			return false;
	}

	// Accique channel prority.
	if (!m_trackManager->acquireChannel(CHANNEL_NAME, shared_from_this(), CHANNEL_INTERFACE_NAME)) {
		AISDK_ERROR(LX("executeRecognizeFailed").d("reason", "Unable to acquire channel"));
		//executeResetState();
		return false;
    }
#if 0

	if(m_readerThread.joinable()) {
		m_readerThread.join();
	}
#endif	
	// Formally update state now.
	setState(SoundAiObserver::State::RECOGNIZING);

	/**
	 * It must be ensured that there is a timeout mechanism when there is no response for 
	 * a long time during the transition from recognition to thinking.
	 */
	tryEntryIdleStateOnTimer();
	
	return true;
}

void SoundAiAutomaticSpeechRecognizer::transitionFromListeningTimedOut() {
	m_executor.submit([this]() {
		AISDK_DEBUG0(LX("transitionFromListeningTimedOut"));
		set_unwakeup_status();;
		///executeResetState();
	});
}

void SoundAiAutomaticSpeechRecognizer::tryEntryIdleStateOnTimer() {
	if(!m_timeoutForListeningTimer.start(
		ASR_TIMEOUT,
		std::bind(&SoundAiAutomaticSpeechRecognizer::transitionFromListeningTimedOut, this))
		.valid()) {
		AISDK_ERROR(LX("tryEntryIdleStateOnTimer").d("reason", "failedToStartTryEntryIdleStateOnTimer"));
	}
}

void SoundAiAutomaticSpeechRecognizer::terminate() {

}


}	//namespace engine
}	//namespace soundai
} //namespace aisdk
