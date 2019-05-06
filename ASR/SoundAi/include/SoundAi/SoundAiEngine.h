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
 
#ifndef __SOUNDAI_ENGINE_H_
#define __SOUNDAI_ENGINE_H_
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>

#include <Utils/SoundAi/SoundAiObserverInterface.h>
#include <Utils/DeviceInfo.h>
#include <DMInterface/MessageConsumerInterface.h>

#include "SoundAi/sai_sdk.h"

namespace aisdk{
namespace soundai {
namespace engine {
	
class SoundAiEngine {
public:
	using SoundAiObserver = utils::soundai::SoundAiObserverInterface;
	static std::unique_ptr<SoundAiEngine> create(
		const std::shared_ptr<utils::DeviceInfo>& deviceInfo,
		std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
		const std::string &configPath, double threshold = 0.45);

	/**
	 * Start service.
	 */
	bool start();

	/**
	 * Stop service.
	 */	
	bool stop();

	/**
     * Adds an observer to be notified of sound ai state changes.
     *
     * @param observer The new observer to notify of sai state changes.
     */
    void addObserver(std::shared_ptr<utils::soundai::SoundAiObserverInterface> observer);

    /**
     * Removes an observer from the internal collection of observers synchronously. If the observer is not present,
     * nothing will happen.
     *
     * @param observer The observer to remove.
     */
    void removeObserver(std::shared_ptr<utils::soundai::SoundAiObserverInterface> observer);
	
    /**
     * Destructor.
     */
	~SoundAiEngine();
	
protected:
	/// Pointing to the class itself @c SoundAiEngine and pass to static function
	static SoundAiEngine *m_soundAiEngine;

private:
	/**
     * Constructor.
     */
	SoundAiEngine(
		const std::shared_ptr<utils::DeviceInfo>& deviceInfo,
		std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
		const std::string &configPath, double threshold);

    /**
     * Initialize the new instance.
     *
     * @return Whether or not the operation was successful.
     */
    bool init();

	/**
     * This function updates the @c SoundAiObserver state and notifies the state observer.  Any changes to
     * @c m_state should be made through this function.
     *
     * @param state The new state to change to.
     */
    void setState(SoundAiObserver::State state);

	/**
	 * Notifies all keyword observers of the keyword detection.
	 *
	 * @param dialogId The stream in which the keyword was detected.
	 * @param keyword The keyword detected.
	 * @param angle Indicate wake-up location.
	 */
	void notifyKeyWordObservers( 
		std::string dialogId,
		std::string keyword,
		float angle);
	
	///@{ sai sdk interface method
	/**
	 * Call-back functional handler
	 * @param id: dialog id
	 * @param buffer: preprocessed asr data for transfering
	 * @param size: buffer length
	 *
	 * Note: When you got data, copy it for transfering to sai online server for postprocessing
	 */
	static void asrDataCallback(void * usr_data_asr, const char * id, const char *buffer, size_t size);

	/**
	 * Call-back functional handler
	 * @param id: dialog id
	 * @param key_word: key word
	 * @param score: wake belief level
	 * @param wakeup_result: wake keyword index
	 * @param angle: angle value
	 * @param usr_set: denoting if wakeup is by intentional user setting
	 * @return 0 denotes user allows sdk to be waken up
	 * Note: When sdk is waken up, this functional handler is called back.
	 * When `id` and `key_word` are empty, user should set the return value to let sdk truly wake up (generate new dialog id) or not
	 */
	static int wakeupCallback(void * usr_data_wk, const char * id, const char * key_word,
	                           float score, int wakeup_result, float angle, int usr_set);

	/**
	 * Call-back functional handler
	 * @param id: dialog id
	 * @param buffer: wakeup data (currently mono-channel, 16KHz) for transfering
	 * @param size: buffer length
	 *
	 * Note: When you got this wakeup data, copy it for transfering to sai wake-learning online server
	 */
	static void ivwDataCallback(const char * id, const char *buffer, size_t size);

	/**
	 * Call-back functional handler
	 * @param buffer: voip data (currently mono-channel, 16KHz)
	 * @param size: buffer length
	 *
	 * Note: When set up voip-mode, you would be feed with this voip data, copy it for further usage
	 */
	static void voipDataCallback(void * usr_data_voip, const char* id, const char *buffer, size_t size);

	/**
	 * Call-back functional handler
	 * @param is_one_shot: normally, it denotes whether the wakeup is in one-shot-mode(1:yes, 0:no);
	 *	 	when voice-print verification is on, its value tells:
	 * SAI_VP_REG_SUCCESS: success, verification runs smoothly
	 * SAI_VP_REG_MORE: need more time for voice recording
	 * SAI_VP_VERIFY_FAILED: failure
	 * also, when some error took place, this handler provides error code like ExternalError defined above
	 */
	static void oneshotCallback(int event_type);

	/**
	 * Call-back functional handler
	 * @param val: voice energy value
	 */
	static void wavEnergyCallback(float val);

	/**
	 * Call-back functional handler
	 * @param id: dialog id, type: event type, error_code: network error code, msg: information message
	 */
	static void netEventCallback(const char * id, int type, int error_code, const char* msg);
	
	/**
	 * Call-back functional handler
	 * @param code: network status, msg: information message
	 */
	static void netStatusCallback(networkStatusCode code, const char* msg);
	
	/**
	 * Call-back functional handler
	 * @param id: dialog id
	 * @param len: sent data length (ms)
	 */
	static void beatDataCallback(const char * id, size_t len);

	///@}
	
	/// Device info
    std::shared_ptr<utils::DeviceInfo> m_deviceInfo;

	/// A consumer which the semantics message has been receive from sai sdk.
	std::shared_ptr<dmInterface::MessageConsumerInterface> m_messageConsumer;
	
	/// resource file path for sai sdk
	std::string m_config;
	
	/// wakeup threshold (0~1) refer to sai sdk 
	double m_threshold;

	/**
	 * voip mode flag
	 * 0: normal mode, 1: voip mode
	 */
	int m_voipMode;

	/// Set log level
	releaseLogLevel m_logLevel;

	/// Set of observers of SoundAiObserverInterface.
	std::unordered_set<std::shared_ptr<utils::soundai::SoundAiObserverInterface>> m_observers;

	/// Mutex to guard access of m_observers.
    std::mutex m_observerMutex;

	/// The current state of the @c SoundAiObserver.
    SoundAiObserver::State m_state;
};

}	//engine
}	//soundai
}// namespace aisdk

#endif //__SOUNDAI_ENGINE_H_

