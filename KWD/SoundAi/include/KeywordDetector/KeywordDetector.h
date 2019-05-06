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

#ifndef _KEYWORD_DETECTOR_H_
#define _KEYWORD_DETECTOR_H_
#include <string>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <Utils/SoundAi/SoundAiObserverInterface.h>
#include <Utils/Channel/AudioTrackManagerInterface.h>

namespace aisdk {
namespace kwd {

/**
*  an KeywordDetector class
*/
class KeywordDetector : public utils::soundai::SoundAiObserverInterface {
public:
	/**
	 * Constructs an empty KeywordDetector.
	 */
	KeywordDetector(std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager);
	
	/**
	* Destructor the KeywordDetector.
	*/
	~KeywordDetector();

	/// name SoundAiObserverInterface methods:
	/// @{
	void onKeyWordDetected(
	 std::string dialogId,
	 std::string keyword,
	 float angle) override;

	void onStateChanged(
		utils::soundai::SoundAiObserverInterface::State state) override;
	/// @}
	
	/// stop a detectionHandler task
	void stop();

private:

	/// The main function that acquire player priority and keyword timeout detect.
	void detectionHandler();

	/// AudioTrackManagerInterface instance to acqurie the channel.
	std::shared_ptr<utils::channel::AudioTrackManagerInterface> m_trackManager;
	
    /// Internal thread that executor acquire player priority and keyword timeout detect task.
    std::thread m_detectionThread;
	
	/// The condition variable used to wait for @c stop() or period timeouts.
    std::condition_variable m_detectionWaitCondition;

    /// The mutex for @c m_waitCondition.
    std::mutex m_detectionWaitMutex;

    /// Flag which indicates that a @c detection handler is active.
    std::atomic<bool> m_running;

    /**
     * Flag which requests that the active @c detectionHandler be stopped.
     * variable.
     */
    bool m_stopping;

};

}	// namespace kwd
}  //namespace aisdk

#endif  // _KEYWORD_DETECTOR_H_

