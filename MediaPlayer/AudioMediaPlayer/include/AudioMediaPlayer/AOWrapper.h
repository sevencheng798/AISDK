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
#ifndef __AOWRAPPER__H_
#define __AOWRAPPER__H_

#include <memory>
#include <mutex>
#include <stdbool.h>

#include <ao/ao.h>
#include <Utils/MediaPlayer/MediaPlayerInterface.h>
#include <Utils/SafeShutdown.h>
#include "FFmpegInputControllerInterface.h"
#include "AudioMediaPlayer/FFmpegDecoder.h"
#include "AOEngine.h"

namespace aisdk {
namespace mediaPlayer{
namespace ffmpeg{

/**
 * This class implements an media player.
 *
 * The implementation uses Libao APIs to play the audio and FFmpeg to decode and resample the media input.
 */	
class AOWrapper 
		: public utils::mediaPlayer::MediaPlayerInterface
		, public utils::SafeShutdown {
public:
    /// Represents one byte of data.
    using Byte = uint8_t;		// add Sven
    
	enum class AOPlayerState {
		/// AOEngine already be initialized but no resources have been requested yet.
		IDLE,
		/// AO resources be qequested but it hasn't started playing yet.
		OPENED,
		/// AO resources start playing.
		PLAYING,
		/// AO resources be requested paused.
		PAUSED,
		/// AO resources already be play finished.
		FINISHED
	};
	
    /**
     * Creates an libao stream object.
     *
     * @param preSampleBits A pointer to the sampleformat type. It shall include at @PaSampleFormat.
     * @param channelsCount represents the stream whether is stereo or mono.
     * @return A pointer to the @c PaWrapper if succeed; @c nullptr otherwise.
     */
	static std::unique_ptr<AOWrapper> create(	
	std::shared_ptr<AOEngine> aoEngine,
	const PlaybackConfiguration& config = PlaybackConfiguration());

    /// @name MediaPlayerInterface methods.
    ///@{
    SourceId setSource(const std::string& url, std::chrono::milliseconds offset) override;
	SourceId setSource(std::shared_ptr<std::istream> stream, bool repeat) override;
	SourceId setSource(
		std::shared_ptr<utils::attachment::AttachmentReader> attachmentReader,
		const utils::AudioFormat* format = nullptr) override;
    bool play(SourceId id) override;
    bool stop(SourceId id) override;
	bool pause(SourceId id) override;
	bool resume(SourceId id) override;
	void setObserver(
		std::shared_ptr<utils::mediaPlayer::MediaPlayerObserverInterface> playerObserver) override;
	///@}
	

	/// Destructor
    ~AOWrapper();
protected:
    /// @name SafeShutdown methods.	
    /// @{
    void doShutdown() override;
	/// @}
private:
	/**
     * Constructor
     */	
    AOWrapper(
    std::shared_ptr<AOEngine> aoEngine,
    const PlaybackConfiguration& config);
		
	/// initialize Portaudio
	bool initialize();

	/**
     * Internal method used to create a new media queue and increment the request id.
     */
	int configureNewRequest(
			std::unique_ptr<FFmpegInputControllerInterface> inputController,
			std::chrono::milliseconds offset = std::chrono::milliseconds(0));

	/// Internal method implements the stop media player logic. This method should be called after acquring @c m_mutex
    bool stopLocked();

	/// Thread audio play handler method.
    void doPlayAudioLoop();

	/* Processing the stream for decoding and playback.
	 * @note This method must only be called by threads @c doPlayAudioLoop() that have acquired @c m_operationMutex.
     *
     * @param lock A @c unique_lock on m_operationMutex, allowing this method to release the lock around callbacks
     * that need to be invoked.
     */
	void doPlayAudioLocked(std::unique_lock<std::mutex> &lock);
	
    /// The current source id.
    SourceId m_sourceId;
	
    /// Keep a pointer to the libao(initialize) engine to guarantee that it doesn't 
    /// get destroyed before other libao objects.
    std::shared_ptr<AOEngine> m_engine;

	/// A decoder object @c FFmpegDecoder.
	std::shared_ptr<FFmpegDecoder> m_decoder;

	/// A specific objects @c ao_device from libao library.
	std::shared_ptr<ao_device> m_device;
		
    /// Save the initial media offset to compute total offset.
    std::chrono::milliseconds m_initialOffset;

	/// A type used to set audio playback parameters.
	//ao_sample_format m_aoSampleformat;
	
	AOPlayerState m_state;

	/// /// Flags whether or not processor task is shutdown.
	bool m_isShuttingDown;
	
	/// A specific default driver id.
	//int m_defaultDriver;
	
    // The android media player configuration.
    PlaybackConfiguration m_config;

	/// Internal thread that process the specify stream to play audio data.
	std::thread m_playerThread;
	
	std::shared_ptr<utils::mediaPlayer::MediaPlayerObserverInterface> m_observer;

	/// The condition variable used to wait @c libao operater.
    std::condition_variable m_playerWaitCondition;
		
    /// Mutex used to synchronize media player operations.
    std::mutex m_operationMutex;
	
	/// Mutex used to synchronize @c request creation.
	//std::mutex m_requestMutex;
};

}//namespace ffmpeg
} //namespace mediaPlayer
} //namespace aisdk
#endif //__AOWRAPPER__H_