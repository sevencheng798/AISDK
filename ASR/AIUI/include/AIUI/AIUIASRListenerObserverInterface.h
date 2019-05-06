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

#ifndef __AIUI_ASR_LISTENER_OBSERVER_INTERFACE_H_
#define __AIUI_ASR_LISTENER_OBSERVER_INTERFACE_H_

#include <memory>

namespace aisdk {
namespace asr {
namespace aiuiEngine {
// A class asr that iflytek aiui to implements the ASR function.
class AIUIASRListenerObserverInterface {
public:
	/**
     * Destructor.
     */
    virtual ~AIUIASRListenerObserverInterface() = default;

	virtual void handleEventStateReady() = 0;

	virtual void handleEventStateWorking() = 0;

	virtual void handleEventVadBegin() = 0;
	/**
	 * Handle the action specified by the nlp result in @c IAIUIEvent.
	 * @params unparsedIntent The unparsed intent(answer) JSON string from AIUI Cloud. 
	 */
	virtual void handleEventResultNLP(const std::string unparsedIntent) = 0;
	
	/**
	 * Handle the action specified by the nlp result in @c IAIUIEvent.
	 * @params unparsedIntent The unparsed intent(answer) JSON string from AIUI Cloud. 
	 */
	virtual void handleEventResultTPP(const std::string unparsedIntent) = 0;
	
	/**
	 * Handle the action specified by the nlp result in @c IAIUIEvent.
	 */
	virtual void handleEventResultTTS(const std::string info, const std::string data) = 0;
	
	/**
	 * Handle the action specified by the nlp result in @c IAIUIEvent.
	 */
	virtual void handleEventResultIAT() = 0;

};

} // namespace aiui
} // namespace asr
} // namespace aisdk
#endif // __AIUI_ASR_LISTENER_OBSERVER_INTERFACE_H_