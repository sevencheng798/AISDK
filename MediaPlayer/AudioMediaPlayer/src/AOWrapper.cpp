/*
 * Copyright 2018 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
// memset references
#include <cstring>

extern "C" {
#include <libavformat/avformat.h>
}

#include <Utils/Logging/Logger.h>
#include <Utils/MediaPlayer/MediaPlayerObserverInterface.h>
#include "AudioMediaPlayer/FFmpegUrlInputController.h"
#include "AudioMediaPlayer/FFmpegStreamInputController.h"
#include "AudioMediaPlayer/FFmpegAttachmentInputController.h"
//#include "AudioMediaPlayer/FFmpegDecoder.h"
#include "AudioMediaPlayer/AOWrapper.h"
#include "AudioMediaPlayer/AudioOutputDeleter.h"

/// String to identify log entries originating from this file.
static const std::string TAG{"AOWrapper"};

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

/// Buffer size for the decoded data. This has to be big enough to be used with the decoder.
static constexpr size_t BUFFER_SIZE{16384u};	// 32768

namespace aisdk {
namespace mediaPlayer {
namespace ffmpeg {
/**
 * Describes the format of audio samples @c ao_sample_format.
 * It will convert the appropriate bits and channels according to config @c PlaybackConfiguration.
*/
static int convertBitsPreSample(PlaybackConfiguration::SampleFormat format) {
	switch (format) {
		case PlaybackConfiguration::SampleFormat::UNSIGNED_8:
			return 8;
		case PlaybackConfiguration::SampleFormat::SIGNED_16:
			return 16;
		case PlaybackConfiguration::SampleFormat::SIGNED_32:
			return 32;
	}

	AISDK_WARN(LX("invalidFormat").d("format", static_cast<int>(format)));
	return 16;
}

std::unique_ptr<AOWrapper> AOWrapper::create(
	std::shared_ptr<AOEngine> aoEngine,
	const PlaybackConfiguration& config){
	if(!aoEngine) {
		AISDK_ERROR(LX("createFailed").d("reason", "aoEngineIsNullptr"));
		return nullptr;
	}
	
	auto player = std::unique_ptr<AOWrapper>(
			new AOWrapper(aoEngine, config));
	if(!player->initialize()){
		AISDK_ERROR(LX("createFailed").d("reason", "initializeFailedAOWrapper"));
		return nullptr;
	}

	return player;
}

AOWrapper::SourceId AOWrapper::setSource(const std::string& url, std::chrono::milliseconds offset){
	auto input = FFmpegUrlInputController::create(url, offset);
	auto newID = configureNewRequest(std::move(input), offset);
	if(utils::mediaPlayer::MediaPlayerInterface::ERROR == newID){
		AISDK_DEBUG5(LX("setSourceFailed").d("type", "url").d("offset(ms)", offset.count()));
	}

	return newID;
}

AOWrapper::SourceId AOWrapper::setSource(std::shared_ptr<std::istream> stream, bool repeat){	
	auto input = FFmpegStreamInputController::create(stream, repeat);
	auto newID = configureNewRequest(std::move(input));
	if(utils::mediaPlayer::MediaPlayerInterface::ERROR == newID){
		AISDK_DEBUG5(LX("setSourceFailed").d("type", "istream").d("repeat", repeat));
	}
	
	return newID;
}

AOWrapper::SourceId AOWrapper::setSource(
    std::shared_ptr<utils::attachment::AttachmentReader> attachmentReader,
    const utils::AudioFormat* format) {
	auto input = FFmpegAttachmentInputController::create(attachmentReader, format);
	auto newID = configureNewRequest(std::move(input));
	if(utils::mediaPlayer::MediaPlayerInterface::ERROR == newID){
		AISDK_DEBUG5(LX("setSourceFailed").d("type", "attachment").d("format", format));
	}

	return newID;
}

bool AOWrapper::play(SourceId id)
{
	AISDK_DEBUG2(LX(__func__).d("requestId", id));
	std::lock_guard<std::mutex> lock{m_operationMutex};

	if (id != m_sourceId)
		return false;

	if(m_state == AOPlayerState::OPENED) {
		m_state = AOPlayerState::PLAYING;
		AISDK_DEBUG2(LX("PLAY").d("reason", "startPlaySuccess"));
		m_playerWaitCondition.notify_one();

		if (m_observer) {
            m_observer->onPlaybackStarted(m_sourceId);
        }
		 
		return true;
	}else{
		AISDK_DEBUG2(LX("playFailed").d("reason", "dontAgainStartPlay"));
	}
	
	return false;
}

bool AOWrapper::stopLocked(){

	if(m_state != AOWrapper::AOPlayerState::IDLE) {
		m_state = AOWrapper::AOPlayerState::FINISHED;
		if(m_decoder)
			m_decoder->abort();
		m_decoder.reset();
		AISDK_DEBUG2(LX("stopLocked").d("reason", "startStopSuccess"));
		m_playerWaitCondition.notify_one();
		
		if (m_observer) {
            m_observer->onPlaybackStopped(m_sourceId);
        }
		 
		return true;		
	}else {
		AISDK_DEBUG2(LX("stopLockedFailed").d("reason", "alreadyIsStopped"));
	}
	
	return false;
}

bool AOWrapper::stop(SourceId id)
{
	AISDK_DEBUG2(LX(__func__).d("requestId", id));
	std::lock_guard<std::mutex> lock{m_operationMutex};
	if (id == m_sourceId)
		return stopLocked();

	AISDK_ERROR(LX("stopFailed").d("reason", "Invalid Id").d("RequestId", id).d("currentId", m_sourceId));
		
	return false;
}

bool AOWrapper::pause(SourceId id)
{
	AISDK_DEBUG2(LX(__func__).d("requestId", id));
    std::lock_guard<std::mutex> lock{m_operationMutex};
	if (id != m_sourceId)
		return false;
	if(m_state == AOPlayerState::PLAYING) {
		m_state = AOPlayerState::PAUSED;
		AISDK_DEBUG2(LX("PAUSE").d("reason", "startPauseSuccess"));

       if (m_observer) {
           m_observer->onPlaybackPaused(m_sourceId);
       }

	   return true;
	}else{
		AISDK_DEBUG2(LX("pauseFailed").d("reason", "streamIsnotPlaying"));
	}

	return false;
}

bool AOWrapper::resume(SourceId id){

	AISDK_DEBUG2(LX(__func__).d("requestId", id));
    std::lock_guard<std::mutex> lock{m_operationMutex};	
	if (id != m_sourceId)
		return false;

	if(m_state == AOPlayerState::PAUSED) {
		m_state = AOPlayerState::PLAYING;
		AISDK_DEBUG2(LX("RESUME").d("reason", "startResumeSuccess"));
		m_playerWaitCondition.notify_one();

		if (m_observer) {
            m_observer->onPlaybackResumed(m_sourceId);
        }

		return true;
	}else{
		AISDK_DEBUG2(LX("resumeFailed").d("reason", "streamIsnotPaused"));
	}
	
	return false;
}

void AOWrapper::setObserver(std::shared_ptr<utils::mediaPlayer::MediaPlayerObserverInterface> playerObserver){
	std::lock_guard<std::mutex> lock{m_operationMutex};
    m_observer = playerObserver;
}

int AOWrapper::configureNewRequest(
	std::unique_ptr<FFmpegInputControllerInterface> inputController,
	std::chrono::milliseconds offset){
	if(!inputController) {
		AISDK_ERROR(LX("configureNewRequestFailed").d("reason", "inputControllerIsNullptr"));
		return utils::mediaPlayer::MediaPlayerInterface::ERROR;
	}

	{
		// Use global lock to stop player and set new source id.
        std::lock_guard<std::mutex> lock{m_operationMutex};
        stopLocked();
        m_sourceId++;
        m_initialOffset = offset;
		m_state = AOPlayerState::OPENED;
	}

	// Make sure current thread be destroyed.
	if(m_playerThread.joinable()) {
		m_playerWaitCondition.notify_one();	//
		m_playerThread.join();
	}

	// Delete old decoder before configuring new one.
#if 0
	if(m_decoder)
		m_decoder->abort();
#endif		
	m_decoder.reset();
	
	m_decoder = FFmpegDecoder::create(std::move(inputController), m_config);

	m_playerThread = std::thread(&AOWrapper::doPlayAudioLoop, this);

	return m_sourceId;
}

bool AOWrapper::initialize(){

	ao_sample_format format;
	// Bzero the structs @c ao_sample_format.
	std::memset(&format, 0, sizeof format);
	format.bits = convertBitsPreSample(m_config.sampleFormat());
	format.channels = m_config.numberChannels();
	format.rate = m_config.sampleRate();
	format.byte_format = (m_config.isLittleEndian()?AO_FMT_LITTLE: AO_FMT_BIG);

	auto defaultDriver = m_engine->getDefaultDriver();
	
	/// Open liao driver.
	auto device = ao_open_live(defaultDriver, &format, NULL);
	if (device == NULL) {
		AISDK_DEBUG2(LX("initializeFailed").d("reason", "ErrorOpeningDevice"));
		return false;
	}

	m_device = std::shared_ptr<ao_device>(device, AOOpenLiveDeleter());
	
	return true;
}

void AOWrapper::doShutdown()
{
	{
		std::lock_guard<std::mutex> lock{m_operationMutex};
		m_isShuttingDown = true;
		stopLocked();
	    m_observer.reset();
#if 0
		if(m_decoder)
			m_decoder->abort();
#endif
	    m_sourceId = ERROR;
	}
	// Make sure current thread be destroyed.
	if(m_playerThread.joinable()) {		
		m_playerWaitCondition.notify_one();	//
		m_playerThread.join();
	}

	if(m_device) {
		m_device.reset();
	}
}

/* The instance callback, where we have access to every method/variable in object of class Sine */
void AOWrapper::doPlayAudioLoop() {
	auto task = [this](){
		return (m_state == AOPlayerState::PLAYING) || (m_state == AOPlayerState::FINISHED) || m_isShuttingDown;
	};

	std::unique_lock<std::mutex> lock(m_operationMutex);
	while(true) {
		m_playerWaitCondition.wait(lock, task);
		if(m_isShuttingDown || m_state == AOPlayerState::FINISHED) {
			m_state = AOPlayerState::IDLE;
			break;
		}
		
		doPlayAudioLocked(lock);
	}
}

void AOWrapper::doPlayAudioLocked(std::unique_lock<std::mutex> &lock) {
	bool unexpected = false;

	// We need to release the lock @c m_operationMutex when the @c FFMpegDecoder enters the decoding stage and play decode data.
	lock.unlock();

	do {
	
		size_t wordsRead;
		DecoderInterface::Status status;
		Byte buffer[BUFFER_SIZE];
		/// Start to read and decode a new frame
		std::tie(status, wordsRead) = m_decoder->read(buffer, sizeof buffer);
		if(DecoderInterface::Status::ERROR == status) {
			AISDK_ERROR(LX("doPlayAudioLockedFailed").d("reason", "decodingFailed"));
			unexpected = true;
			break;
		}

		if(DecoderInterface::Status::DONE == status) {
			AISDK_DEBUG2(LX("doPlayAudioLockedDone").d("reason", "decodingFinished"));
			unexpected = true;
			break;
		}
		
	//	 std::cout << "decodec size: " << wordsRead << std::endl;

		if(ao_play(m_device.get(), (char *)buffer, wordsRead) == 0) {
			AISDK_ERROR(LX("doPlayAudioLockedDone").d("reason", "ao_play failed"));
		}
	}while(0);
	
	lock.lock();
	if(unexpected) {
		m_state = AOWrapper::AOPlayerState::FINISHED;
		if (m_observer) {
            m_observer->onPlaybackFinished(m_sourceId);
        }
	}
	
}

AOWrapper::AOWrapper(
	std::shared_ptr<AOEngine> aoEngine,
	const PlaybackConfiguration& config) :
	SafeShutdown{"AOWrapper"},
	m_sourceId{utils::mediaPlayer::MediaPlayerInterface::ERROR},
	m_engine{aoEngine},
	m_decoder{nullptr},
	m_device{nullptr},
	m_initialOffset{0},
	m_state{AOPlayerState::IDLE},
	m_isShuttingDown{false},
	m_config{config} {
	
}

AOWrapper::~AOWrapper() {
	AISDK_DEBUG5(LX("AOWrapper").d("reason", "Destructor"));
	m_decoder.reset();
	doShutdown();
}

}// namespace ffmpeg
} //namespace mediaPlayer
} //namespace aisdk
