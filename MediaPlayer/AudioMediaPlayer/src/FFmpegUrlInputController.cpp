/*
 * Copyright 2018 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/common.h>
#include <libavutil/error.h>
}

#include <Utils/Logging/Logger.h>
#include "AudioMediaPlayer/FFmpegDeleter.h"
#include "AudioMediaPlayer/FFmpegUrlInputController.h"

/// String to identify log entries originating from this file.
static const std::string TAG("FFmpegUrlInputController");

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace mediaPlayer {
namespace ffmpeg {

/// This string represents the FFmpeg HTTP User-Agent option key.
static const char* USER_AGENT_OPTION{"user_agent"};

std::unique_ptr<FFmpegUrlInputController> FFmpegUrlInputController::create(
    const std::string& url,
    const std::chrono::milliseconds& offset) {
	std::ostringstream oss;
    if (url.empty()) {
		AISDK_ERROR(LX("createFailed").d("reason", "emptyOriginalUrl"));
        return nullptr;
    }

	AISDK_DEBUG5(LX("created").d("url", url).d("offset(ms)", offset.count()));
	
    auto controller = std::unique_ptr<FFmpegUrlInputController>(new FFmpegUrlInputController(url, offset));
    if (!controller->findFirstEntry()) {
		AISDK_ERROR(LX("createFailed").d("reason", "emptyPlayList"));
        return nullptr;
    }

    return controller;
}

bool FFmpegUrlInputController::hasNext() const {
    return !m_done;
}

bool FFmpegUrlInputController::next() {
    if (m_done) {
		AISDK_INFO(LX("nextFailed").d("reason", "finished"));
        return false;
    }

	/*To-Do*/
    return true;
}

bool FFmpegUrlInputController::findFirstEntry() {
    //auto offset = m_offset;
    //while (!m_done) {

    //}

	/* To-Do fix me: to prase playlist or m3u*/
	// we'd better to implatement how to prase speical mutex.
	m_done = true;

    return true;
}

FFmpegUrlInputController::~FFmpegUrlInputController() {
}

FFmpegUrlInputController::FFmpegUrlInputController(
	const std::string& url,
    const std::chrono::milliseconds& offset) :
    	m_currentUrl{url},
        m_offset{offset},
        m_done{false},
        m_avFormatContext{nullptr} {
}

AVFormatContext* FFmpegUrlInputController::createNewFormatContext() {
    if (m_currentUrl.empty()) {
		AISDK_ERROR(LX("createNewContextFailed").d("reason", "emptyUrl"));
       	return nullptr;
    }

     m_avFormatContext = avformat_alloc_context();
    if (!m_avFormatContext) {
		AISDK_ERROR(LX("createNewContextFailed").d("reason", "avFormatAllocFailed"));
        return nullptr;
    }

	return m_avFormatContext;
}

std::tuple<FFmpegInputControllerInterface::Result, std::shared_ptr<AVFormatContext>, std::chrono::milliseconds>
FFmpegUrlInputController::getCurrentFormatContextOpen() {
	if(!m_avFormatContext) {
		AISDK_ERROR(LX("getContextFailed").d("reason", "avFormatIsnullptr"));
        return std::make_tuple(Result::ERROR, nullptr, std::chrono::milliseconds::zero());
	}

	auto avFormatContext = m_avFormatContext;
	// Clear the original pointer to prepare for the next creation.
	m_avFormatContext = nullptr;

    AVDictionary* options = nullptr;
    av_dict_set(&options, USER_AGENT_OPTION, "AiSdkv1.0.1", 0);
    auto error = avformat_open_input(&avFormatContext, m_currentUrl.c_str(), nullptr, &options);
    auto optionsPtr = std::unique_ptr<AVDictionary, AVDictionaryDeleter>(options);

    if (!optionsPtr) {
        // FFmpeg will put the options that could not be set back into the options dictionary.
        auto option = av_dict_get(optionsPtr.get(), USER_AGENT_OPTION, nullptr, 0);
        if (option) {
            // Do not modify or free the option pointer.
			AISDK_WARN(LX(__func__).d("issue", "unableSetUserAgent").d(option->key, option->value));
        }
    }
    if (error != 0) {
        // The avFormatContext will be freed on failure.
        if (-EAGAIN == error) {
			AISDK_DEBUG5(LX("getContextFailed").d("reason", "Data unavailable. Try again."));
            return std::make_tuple(Result::TRY_AGAIN, nullptr, std::chrono::milliseconds::zero());
        }

        //auto errorStr = av_err2str(error);
		AISDK_ERROR(LX("getContextFailed")
					.d("reason", "openInputFailed")
					.d("url", m_currentUrl));
		
        return std::make_tuple(Result::ERROR, nullptr, std::chrono::milliseconds::zero());
    }

    return std::make_tuple(Result::OK, std::shared_ptr<AVFormatContext>(avFormatContext, AVFormatContextDeleter()), m_offset);
}

}  // namespace ffmpeg
}  // namespace mediaPlayer
} //namespace aisdk
