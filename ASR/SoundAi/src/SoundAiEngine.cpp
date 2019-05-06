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
#include <Utils/SoundAi/SoundAiObserverInterface.h>

#include "SoundAi/SoundAiEngine.h"
#include "SoundAi/NetEventTypes.h"

/// String to identify log entries originating from this file.
static const std::string TAG{"SoundAiEngine"};

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace soundai {
namespace engine {

/// Pointing to the class itself @c SoundAiEngine and pass to static function
SoundAiEngine* SoundAiEngine::m_soundAiEngine = nullptr;

std::unique_ptr<SoundAiEngine> SoundAiEngine::create(
		const std::shared_ptr<utils::DeviceInfo>& deviceInfo,
		std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
		const std::string &configPath, double threshold){

	if(!deviceInfo){
		AISDK_ERROR(LX("CreateFailed").d("reason", "deviceInfoIsNull"));
		return nullptr;
	}

	if(configPath.empty()){
		AISDK_ERROR(LX("CreateFailed").d("reason", "soundAiConfigPathIsNull"));
		return nullptr;
	}

	std::unique_ptr<SoundAiEngine> instance(new SoundAiEngine(deviceInfo, messageConsumer, configPath, threshold));
	if(!instance->init()){
		AISDK_ERROR(LX("CreateFailed").d("reason", "initializeFailed"));
		return nullptr;
	}

	return instance;
}

bool SoundAiEngine::start(){
	int ret = start_service();

	/// Set voip mode
	set_voip_flag(m_voipMode);
	
	return ((ret == 0)? true:false);
}

bool SoundAiEngine::stop(){
	int ret = stop_service();

	return ((ret == 0)? true:false);
}

void SoundAiEngine::addObserver(std::shared_ptr<utils::soundai::SoundAiObserverInterface> observer){
	if(!observer){
		AISDK_ERROR(LX("addObserverFailed").d("reason", "nullobserver"));
		return;
	}

	std::lock_guard<std::mutex> lock{m_observerMutex};
	m_observers.insert(observer);
}

void SoundAiEngine::removeObserver(std::shared_ptr<utils::soundai::SoundAiObserverInterface> observer){
	if(!observer){
		AISDK_ERROR(LX("removeObserverFailed").d("reason", "nullobserver"));
		return;
	}

	std::lock_guard<std::mutex> lock{m_observerMutex};
	m_observers.erase(observer);

}

SoundAiEngine::~SoundAiEngine(){
	AISDK_INFO(LX("Destructor"));
	
	stop();
	terminate_system();
}

SoundAiEngine::SoundAiEngine(
	const std::shared_ptr<utils::DeviceInfo>& deviceInfo,
	std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
	const std::string &configPath, double threshold)
	:m_deviceInfo{deviceInfo}
	,m_messageConsumer{messageConsumer}
	,m_config{configPath}
	,m_threshold{threshold}
	,m_voipMode{0}
	,m_logLevel{SAI_LOGGER_DEBUG}
	,m_state{SoundAiObserver::State::IDLE}{
	m_soundAiEngine = this;
}

bool SoundAiEngine::init(){
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

void SoundAiEngine::asrDataCallback(void * usr_data_asr, const char * id, const char *buffer, size_t size){
	std::cout << __func__ << " :Entry" << std::endl;
	std::cout << "buffer: " << buffer << std::endl;
}

void SoundAiEngine::setState(SoundAiObserver::State state){
	
	std::lock_guard<std::mutex> lock{m_observerMutex};
	if(m_state == state)
		return;

	m_state = state;
	for (auto observer : m_observers) {
        if (observer) {
            observer->onStateChanged(state);
        }
    }
}

void SoundAiEngine::notifyKeyWordObservers( 
	std::string dialogId,
	std::string keyword,
	float angle){
	
	std::lock_guard<std::mutex> lock{m_observerMutex};
	for (auto observer : m_observers) {
        if (observer) {
            observer->onKeyWordDetected(dialogId, keyword, angle);			
        }
    }
}

int SoundAiEngine::wakeupCallback(void * usr_data_wk, const char * id, const char * key_word,
						   float score, int wakeup_result, float angle, int usr_set){

   AISDK_INFO(LX("wakeupCallback")
   				.d("id", id)
   				.d("key_word", key_word)
   				.d("score", score)
   				.d("wakeup_result", wakeup_result)
   				.d("angle", angle)
   				.d("usr_set", usr_set));
   
	SoundAiEngine* engine = static_cast<SoundAiEngine*>(usr_data_wk);
	
	std::string dialogID = id;
	std::string keyword = key_word;

	/// To let sai sdk generate new dialog id
	if(dialogID.empty()){
		return 0;
	}

	/// Notify observer to handler event
	engine->notifyKeyWordObservers(dialogID, keyword, angle);

	/// Comein capture stream state 
	engine->setState(SoundAiObserver::State::RECOGNIZING);
	
	return 0;
}

void SoundAiEngine::ivwDataCallback(const char * id, const char *buffer, size_t size){
	std::cout << __func__ << " :Entry" << std::endl;	
	std::cout << "buffer: " << buffer << std::endl;
}

void SoundAiEngine::voipDataCallback(void * usr_data_voip, const char* id, const char *buffer, size_t size){
	std::cout << __func__ << " :Entry" << std::endl;
	std::cout << "buffer: " << buffer << std::endl;
	// default no-op
}

void SoundAiEngine::oneshotCallback(int event_type){
	std::cout << __func__ << " :Entry" << ": event_type: " << event_type << std::endl;
	// default no-op
}

void SoundAiEngine::wavEnergyCallback(float val){
	//std::cout << __func__ << " :Entry" << std::endl;
	// default no-op
}

void SoundAiEngine::netEventCallback(const char * id, int type, int error_code, const char* msg){
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
		m_soundAiEngine->setState(SoundAiObserver::State::IDLE);

		// To-Do
		// In here parse json data
		// ...
		// ...
		// ...
		m_soundAiEngine->m_messageConsumer->consumeMessage(id, msg);
	}
	
}

void SoundAiEngine::netStatusCallback(networkStatusCode code, const char* msg){
	AISDK_INFO(LX("netStatusCallback").d("code", code).d("msg", msg));

}

void SoundAiEngine::beatDataCallback(const char * id, size_t len){
	//std::cout << __func__ << " :Entry" << std::endl;
	// default no-op
}

}	//namespace engine
}	//namespace soundai
} //namespace aisdk
