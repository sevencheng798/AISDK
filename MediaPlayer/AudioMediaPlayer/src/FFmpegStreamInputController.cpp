/*
 * Copyright 2018 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/common.h>
#include <libavutil/error.h>
}

#include <Utils/Logging/Logger.h>
#include "AudioMediaPlayer/FFmpegDeleter.h"
#include "AudioMediaPlayer/FFmpegStreamInputController.h"

/// String to identify log entries originating from this file.
static const std::string TAG("FFmpegStreamInputController");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace mediaPlayer {
namespace ffmpeg {

/// Buffers will be the size of a regular page.
static constexpr int BUFFER_SIZE{4096};

std::unique_ptr<FFmpegStreamInputController> FFmpegStreamInputController::create(
    std::shared_ptr<std::istream> stream,
    bool repeat) {
    if (!stream) {
		AISDK_ERROR(LX("createFailed").d("reason", "nullStream"));
        return nullptr;
    }

    return std::unique_ptr<FFmpegStreamInputController>(new FFmpegStreamInputController(stream, repeat));
}

int FFmpegStreamInputController::read(uint8_t* buffer, int bufferSize) {
    m_stream->read(reinterpret_cast<char*>(buffer), bufferSize);
    auto bytesRead = m_stream->gcount();
    if (!bytesRead) {
        if (m_stream->bad()) {
			AISDK_ERROR(LX("readFailed").d("readState", m_stream->rdstate()));
            return AVERROR_EXTERNAL;
        }
        return AVERROR_EOF;
    }
    return bytesRead;
}

bool FFmpegStreamInputController::hasNext() const {
    return m_repeat;
}

bool FFmpegStreamInputController::next() {
    if (!m_repeat) {
		AISDK_ERROR(LX("nextFailed").d("reason", "repeatIsOff"));
        return false;
    }

    m_stream->clear();
    m_stream->seekg(0);
    return m_stream->good();
}

FFmpegStreamInputController::FFmpegStreamInputController(std::shared_ptr<std::istream> stream, bool repeat) :
        m_stream{stream},
        m_repeat{repeat} {
}


AVFormatContext* FFmpegStreamInputController::createNewFormatContext() {

     m_avFormatContext = avformat_alloc_context();
    if (!m_avFormatContext) {
		AISDK_ERROR(LX("createNewContextFailed").d("reason", "avFormatAllocFailed"));
        return nullptr;
    }

	return m_avFormatContext;
}

std::tuple<FFmpegInputControllerInterface::Result, std::shared_ptr<AVFormatContext>, std::chrono::milliseconds>
FFmpegStreamInputController::getCurrentFormatContextOpen() {
    if (m_ioContext) {
        // Invalidate possible references to this object.
        m_ioContext->opaque = nullptr;
    }

    unsigned char* buffer =
        static_cast<unsigned char*>(av_malloc(BUFFER_SIZE + AVPROBE_PADDING_SIZE));  // Owned by m_ioContext
    if (!buffer) {
		AISDK_ERROR(LX("getContextFailed").d("reason", "avMallocFailed"));
		/**
		 * Conditionally-explicit constructors make it possible to construct a tuple in copy-initialization context using list-initialization syntax
		 * std::tuple<int, int> foo_tuple() 
		 * {
			  return {1, -1};  // Error before N4387 - link:http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4387.html
			  return std::make_tuple(1, -1); // Always works
		 * }
		 * so we need to use make_tuple to make a correction
		 */
        return std::make_tuple(Result::ERROR, nullptr, std::chrono::milliseconds::zero());
    }

    m_ioContext = std::shared_ptr<AVIOContext>(
        avio_alloc_context(buffer, BUFFER_SIZE, false, this, feedBuffer, nullptr, nullptr), AVIOContextDeleter());
    if (!m_ioContext) {
		AISDK_ERROR(LX("getContextFailed").d("reason", "avioAllocFailed"));
        return std::make_tuple(Result::ERROR, nullptr, std::chrono::milliseconds::zero());
    }

    // Allocate the AVFormatContext:
    auto avFormatContext = avformat_alloc_context();
    if (!avFormatContext) {
		AISDK_ERROR(LX("getContextFailed").d("reason", "avFormatAllocFailed"));
        return std::make_tuple(Result::ERROR, nullptr, std::chrono::milliseconds::zero());
    }

    avFormatContext->pb = m_ioContext.get();
    avFormatContext->format_probesize = BUFFER_SIZE;

    auto error = avformat_open_input(&avFormatContext, "", nullptr, nullptr);
    if (error != 0) {
        // The avFormatContext will be freed on failure.
        if (-EAGAIN == error) {
			AISDK_ERROR(LX("getContextFailed").d("reason", "Data unavailable. Try again."));
            return std::make_tuple(Result::TRY_AGAIN, nullptr, std::chrono::milliseconds::zero());
        }
		#if 0 //add Sven @2018-12
        auto errorStr = av_err2str(error);
		#endif
		AISDK_ERROR(LX("getContextFailed").d("reason", "openInputFailed"));
        return make_tuple(Result::ERROR, nullptr, std::chrono::milliseconds::zero());
    }

    auto ioContext = m_ioContext;
    return std::make_tuple(Result::OK,
            std::shared_ptr<AVFormatContext>(
                avFormatContext, [ioContext](AVFormatContext* context) { AVFormatContextDeleter()(context); }),
            std::chrono::milliseconds::zero());
}

int FFmpegStreamInputController::feedBuffer(void* userData, uint8_t* buffer, int bufferSize) {
    auto inputController = reinterpret_cast<FFmpegStreamInputController*>(userData);
    if (!inputController) {
		AISDK_ERROR(LX("getContextFailed").d("reason", "nullInputController"));
        return AVERROR_EXTERNAL;
    }
    return inputController->read(buffer, bufferSize);
}

FFmpegStreamInputController::~FFmpegStreamInputController() {
    if (m_ioContext) {
        m_ioContext->opaque = nullptr;
    }
}

}  // namespace ffmpeg
}  // namespace mediaPlayer
} //namespace aisdk
