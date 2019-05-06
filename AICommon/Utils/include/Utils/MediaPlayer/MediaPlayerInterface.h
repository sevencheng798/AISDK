/*
 * Copyright 2018 its affiliates. All Rights Reserved.
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

#ifndef __MEDIAPLAYERINTERFACE_H_
#define __MEDIAPLAYERINTERFACE_H_

#include <chrono>
#include <cstdint>
#include <future>
#include <memory>
#include <Utils/Attachment/AttachmentReader.h>
#include <Utils/AudioFormat.h>

namespace aisdk {
namespace utils{
namespace mediaPlayer {

/// Represents offset returned when MediaPlayer is in an invalid state.
static const std::chrono::milliseconds MEDIA_PLAYER_INVALID_OFFSET{-1};

class MediaPlayerObserverInterface;

class MediaPlayerInterface {
public:
    /// A type that identifies which source is currently being operated on.
    using SourceId = uint64_t;

    /// An @c SourceId used to represent an error from calls to @c setSource().
    static const SourceId ERROR = 0;

    /**
     * Destructor.
     */
    virtual ~MediaPlayerInterface() = default;
	/**
     * Set an @c url source to play. The source should be set before making calls to any of the playback control
     * APIs. If any source was set prior to this call, that source will be discarded.
	 *
     * @param url The url to set as the source.
     * @param offset An optional offset parameter to start playing from when a @c play() call is made.
     */
    virtual SourceId setSource(
        const std::string& url,
        std::chrono::milliseconds offset = std::chrono::milliseconds::zero()) = 0;
    /**
     * Set an @c istream source to play. The source should be set before making calls to any of the playback control
     * APIs. If any source was set prior to this call, that source will be discarded.
     *
     * @param stream Object from which to read an incoming audio stream.
     * @param repeat Whether the audio stream should be played in a loop until stopped.
     *
     * @return The @c SourceId that represents the source being handled as a result of this call. @c ERROR will be
     * returned if the source failed to be set.
     */		
	virtual SourceId setSource(std::shared_ptr<std::istream> stream, bool repeat) = 0;

	/**
     * Set an @c url source to play. The source should be set before making calls to any of the playback control
     * APIs. If any source was set prior to this call, that source will be discarded.
	 *
     * @param url The url to set as the source.
     * @param offset An optional offset parameter to start playing from when a @c play() call is made.
     */
    virtual SourceId setSource(
        std::shared_ptr<utils::attachment::AttachmentReader> attachmentReader,
        const utils::AudioFormat* format = nullptr) = 0;
		
	/**
     * Starts playing audio specified by the @c setSource() call.
	 */
	virtual bool play(SourceId id) = 0;
	
	/**
     * Stops playing the audio specified by the @c setSource() call.
	 */
	virtual bool stop(SourceId id) = 0;
	
	/**
     * Pauses playing audio specified by the @c setSource() call.
	 */
	virtual bool pause(SourceId id) = 0;
	
	/**
     * Resumes playing the paused audio specified by the @c setSource() call.
	 */
	virtual bool resume(SourceId id) = 0;
	
	/**
     * Returns the offset, in milliseconds, of the media source.
     *
     * @param id The id of the source on which to operate.
     *
     * @return If the specified source is playing, the offset in milliseconds that the source has been playing
     *      will be returned. If the specified source is not playing, the last offset it played will be returned.
     */
    //virtual std::chrono::milliseconds getOffset(SourceId id) = 0;
	
	/**
     * Returns the number of bytes queued up in the media player buffers.
     *
     * @return The number of bytes currently queued in this MediaPlayer's buffer.
     */
    //virtual uint64_t getNumBytesBuffered() = 0;
	
	/**
     * Sets an observer to be notified when playback state changes.
     *
     * @param playerObserver The observer to send the notifications to.
     */
    virtual void setObserver(
        std::shared_ptr<MediaPlayerObserverInterface> playerObserver) = 0;
};
}  // namespace mediaPlayer
}  // namespace utils
} //namespace aisdk
#endif  // __MEDIAPLAYERINTERFACE_H_
