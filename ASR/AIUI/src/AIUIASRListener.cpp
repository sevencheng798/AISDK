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
// jsoncpp ver.1.8.3
#include <json/json.h>

#include <Utils/Logging/Logger.h>
#include "AIUI/AIUIASRListener.h"

/// String to identify log entries originating from this file.
static const std::string TAG{"AIUIASRListener"};
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace asr {
namespace aiuiEngine {

void AIUIASRListener::setObserver(std::shared_ptr<AIUIASRListenerObserverInterface> listenerObserver) {
	std::lock_guard<std::mutex> lock(m_listenerObserverMutex);
	m_listenerObserver = listenerObserver;
}

void AIUIASRListener::setVaildVad(bool newVad) const {
	m_isVad = newVad;
}

bool AIUIASRListener::isVaildVad() {
	return m_isVad;
}

void AIUIASRListener::onEvent(const aiui::IAIUIEvent& event) const {
	switch (event.getEventType()) {
	//SDK 状态回调
	case aiui::AIUIConstant::EVENT_STATE:
	{
		switch (event.getArg1()) {
		case aiui::AIUIConstant::STATE_IDLE:
			AISDK_INFO(LX("EVENT_STATE").d("state", "STATE_IDLE"));
			break;
		case aiui::AIUIConstant::STATE_READY:
			AISDK_INFO(LX("EVENT_STATE").d("state", "STATE_READY"));
			if(m_listenerObserver)
				m_listenerObserver->handleEventStateReady();
			break;
		case aiui::AIUIConstant::STATE_WORKING:
			AISDK_INFO(LX("EVENT_STATE").d("state", "STATE_WORKING"));
			if(m_listenerObserver)
				m_listenerObserver->handleEventStateWorking();
			break;
		}
	}
	break;
	//唤醒事件回调
	case aiui::AIUIConstant::EVENT_WAKEUP:
		AISDK_INFO(LX("EVENT_WAKEUP").d("info", event.getInfo()));
		break;	
	//休眠事件回调
	case aiui::AIUIConstant::EVENT_SLEEP:
		AISDK_INFO(LX("EVENT_SLEEP").d("arg1", event.getArg1()));
		break;
	//VAD事件回调，如找到前后端点
	case aiui::AIUIConstant::EVENT_VAD:
	{
		switch (event.getArg1()) {
		case aiui::AIUIConstant::VAD_BOS:
			AISDK_INFO(LX("EVENT_VAD").d("state", "VAD_BOS"));
			if(m_listenerObserver) {
				m_listenerObserver->handleEventVadBegin();
			}
			break;
		case aiui::AIUIConstant::VAD_EOS:
			AISDK_INFO(LX("EVENT_VAD").d("state", "VAD_EOS"));
			break;
		case aiui::AIUIConstant::VAD_VOL:
			break;
		}
	}
	break;
	//最重要的结果事件回调
	case aiui::AIUIConstant::EVENT_RESULT:
	{
		Json::CharReaderBuilder readerBuilder;
		JSONCPP_STRING errs;
		Json::Value root;
		std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());
		std::string info = event.getInfo();
		if (!reader->parse(info.c_str(), info.c_str()+info.length(), &root, &errs)) {
			AISDK_ERROR(LX("EVENT_RESULT").d("reason", "parseInfoError"));
			break;
		}
		Json::Value data = root["data"][0];
		Json::Value params = data["params"];
		Json::Value content = (data["content"])[0];
		std::string sub = params["sub"].asString();
		//sub字段表示结果的类别，如iat听写，nlp语义结果	
		if(sub != "tts")
			AISDK_INFO(LX("EVENT_RESULT").d("sub", sub));
		Json::Value empty;
		Json::Value contentId = content.get("cnt_id", empty);

		if (contentId.empty()) {
			AISDK_ERROR(LX("EVENT_RESULT").d("reason", "contentIdIsEmpty"));
			break;
		}

		std::string cnt_id = contentId.asString();
		int dataLen = 0;
		const char* buffer = event.getData()->getBinary(cnt_id.c_str(), &dataLen);
		if(NULL == buffer) {
			AISDK_ERROR(LX("EVENT_RESULT").d("reason", "getBinaryIsNULL"));
			break;
		}
		
		std::string resultStr = std::string(buffer, dataLen);
		//bool vad = true;
		if(sub == "tpp") {
			if(m_listenerObserver)
				m_listenerObserver->handleEventResultTPP(resultStr);			
		} else if (sub == "tts") {
			if(m_listenerObserver)
				m_listenerObserver->handleEventResultTTS(info, resultStr);

		} else if (sub == "nlp") {
			setVaildVad(true);
			if(m_listenerObserver)
				m_listenerObserver->handleEventResultNLP(resultStr);

		} else { // iat
			AISDK_DEBUG5(LX("EVENT_RESULT").d("getBinary", resultStr));
		}
	}
	}
}

} // namespace aiui
} // namespace asr
} // namespace aisdk
