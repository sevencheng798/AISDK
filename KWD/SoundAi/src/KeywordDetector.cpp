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
#include "SoundAi/sai_sdk.h"

#include "KeywordDetector/KeywordDetector.h"

namespace aisdk {
namespace kwd {

/// Set keyword timeout time
auto timeoutForListeningToIdle = std::chrono::seconds{15};

void KeywordDetector::stop() {
    {
        std::lock_guard<std::mutex> lock(m_detectionWaitMutex);
        if (m_running) {
            m_stopping = true;
        }
        m_detectionWaitCondition.notify_all();
    }

    if (std::this_thread::get_id() != m_detectionThread.get_id() && m_detectionThread.joinable()) {
        m_detectionThread.join();
		std::cout << "stop task..." << std::endl;  //tmp debug
    }
}

void KeywordDetector::detectionHandler(){
    // Timepoint to measure delay/period against.
    auto now = std::chrono::steady_clock::now();

	/* To-Do */
	/// In here acquire channel player priority
	// ...
	// ...
	
	std::cout << "detectionHandler entry...." << std::endl;
	std::unique_lock<std::mutex> lock(m_detectionWaitMutex);
    // Wait for stop() or a delay/period to elapse.
    if (m_detectionWaitCondition.wait_until(lock, now + timeoutForListeningToIdle, [this]() { return m_stopping; })) {
        m_stopping = false;
        m_running = false;
        return;
    }else{
		std::cout << "wakeup handler timeout:reason: don't check vad event or network unreachable." << std::endl;
		// To-Do release channel player
		// ...
		// ...
	}

	/// Close current wakeup event when a timeout happens
	set_unwakeup_status();
	m_stopping = false;
    m_running = false;
}

void KeywordDetector::onKeyWordDetected(
	 std::string dialogId,
	 std::string keyword,
	 float angle){
	std::cout << __func__ << ": dialogid: " << dialogId << ", word: " << keyword << ", angle: " << angle << std::endl;
#if 0
	if(keyword == "xiaokangxiaokang")
		std::cout << "This isn't our expectation keyword" << std::endl;
#endif

	if(m_detectionThread.joinable()){
		std::cout << "detection stop old task...." << std::endl;
		m_detectionThread.join();
	}
	
	if(m_running.exchange(true)){
		std::cout << "detectHandler already actived..." << std::endl;
		return;
	}
	
	// acquire player priority and timeout count
	 m_detectionThread = std::thread(&KeywordDetector::detectionHandler, this);
}

void KeywordDetector::onStateChanged(utils::soundai::SoundAiObserverInterface::State state){
	std::cout << "====>" << __func__ << ", state change" << std::endl;

	if(utils::soundai::SoundAiObserverInterface::State::BUSY == state){
		stop();
	}
}

KeywordDetector::KeywordDetector(
	std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager)
	: m_trackManager{trackManager}
	, m_running(false)
	, m_stopping(false) {
}

KeywordDetector::~KeywordDetector(){
}


}	// namespace kwd
}  //namespace aisdk

