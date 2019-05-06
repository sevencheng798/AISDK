/*
 * Copyright 2019 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef __AIUI_ASR_LISTENER_H_
#define __AIUI_ASR_LISTENER_H_

#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

#include "aiui/AIUI.h"
#include "FileUtil.h"	// To support read data from file.

#include <Utils/Threading/Executor.h>
#include "AIUI/AIUIASRListenerObserverInterface.h"
//#include "ASR/AutomaticSpeechRecognizerConfiguration.h"

namespace aisdk {
namespace asr {
namespace aiuiEngine {
// A class Listener that handle respond data.
class AIUIASRListener : public aiui::AIUIListener {
public:
	enum class AIUIState {
		IDLE,
		LISTENING,
		SPEAKING
	};
	/**
     * Sets an observer to be notified when have state changes on @c onEvent() be called.
     *
     * @param listenerObserver The observer to send the notifications to.
     */
	void setObserver(std::shared_ptr<AIUIASRListenerObserverInterface> listenerObserver);

	/// Set the vad flag @c m_isVad state.
	void setVaildVad(bool newVad) const;

	/// Returns whether or not the interaction is checked vad.
	bool isVaildVad();
	
	~AIUIASRListener() = default;
protected:

	/// name AIUIListener method:
	/// @{
	void onEvent(const aiui::IAIUIEvent& event) const override;
	/// }
private:
	/**
	 * Constructor.
	 */
#if 0	 
	AIUIASRListener(
		const std::string &appId,
		const std::string &aiuiConfigFile,
		const std::string &aiuiDir,
		const std::string &aiuiLogDir);

#endif
	mutable std::atomic<bool> m_isVad;

	std::shared_ptr<AIUIASRListenerObserverInterface> m_listenerObserver;

	/// Lock to protect m_listenerObserver when users wish to set or remove observers.
    std::mutex m_listenerObserverMutex;
		
	/// The current state of the @c AudioInputProcessor.
    ///ObserverInterface::State m_state;

	//std::shared_ptr<utils::sharedbuffer::Reader> m_reader;
		
	//FileUtil::DataFileHelper* mFileHelper;
	

	//utils::threading::Executor m_executor;
};

} // namespace aiui
} // namespace asr
} // namespace aisdk
#endif // __AIUI_ASR_LISTENER_H_