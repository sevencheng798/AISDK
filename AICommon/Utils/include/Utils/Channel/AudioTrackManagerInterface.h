/*
 * Copyright 2019 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef _AUDIO_TRACK_MANAGER_INTERFACE_H_
#define _AUDIO_TRACK_MANAGER_INTERFACE_H_

#include <future>
#include <memory>
#include <string>

#include "ChannelObserverInterface.h"
#include "AudioTrackManagerObserverInterface.h"

namespace aisdk {
namespace utils {
namespace channel {

/**
 * A AudioTrack takes requests to inquire and release Channels and updates the track of other Channels based on
 * their priorities so that the invariant that there can only be one Foreground Channel is held. The following
 * operations are provided:
 */
class AudioTrackManagerInterface {
public:
    /// The default Dialog Channel name.
    static constexpr const char* DIALOG_CHANNEL_NAME = "Dialog";

    /// The default dialog Channel priority.
    static constexpr unsigned int DIALOG_CHANNEL_PRIORITY = 100;

    /// The default Alarm Channel name.
    static constexpr const char* ALARMS_CHANNEL_NAME = "Alarm";

    /// The default Alarm Channel priority.
    static constexpr unsigned int ALARMS_CHANNEL_PRIORITY = 200;

    /// The default Media Channel name.
    static constexpr const char* MEDIA_CHANNEL_NAME = "Media";

    /// The default Media Channel priority.
    static constexpr unsigned int MEDIA_CHANNEL_PRIORITY = 300;

    /// Destructor.
    virtual ~AudioTrackManagerInterface() = default;

    /**
     * This method will inquire the channel and grant the appropriate track to it and other channels if needed.
     *
     * @param channelName The name of the Channel to acquire.
     * @param channelObserver The observer that will be acquiring the Channel and be notified of focus changes.
     * @param interface The name of the NLP domain interface occupying the Channel. This should be unique and represents the
     * name of the NLP domain interface using the Channel.
     *
     * @return Returns @c true if the Channel can be acquired and @c false otherwise.
     */
    virtual bool acquireChannel(
        const std::string& channelName,
        std::shared_ptr<ChannelObserverInterface> channelObserver,
        const std::string& interface) = 0;

    /**
     * This method will release the Channel and notify the observer of the Channel.
     *
     * @param channelName The name of the Channel to release.
     * @param channelObserver The observer to be released from the Channel.
     * @return @c std::future<bool> which will contain @c true if the Channel can be released and @c false otherwise.
     */
    virtual std::future<bool> releaseChannel(
        const std::string& channelName,
        std::shared_ptr<ChannelObserverInterface> channelObserver) = 0;

    /**
     * This method will request that the currently foregrounded Channel activity be stopped.
     */
    virtual void stopForegroundActivity() = 0;

    /**
     * Add an observer to the focus manager.
     *
     * @param observer The observer to add.
     */
    virtual void addObserver(
        const std::shared_ptr<AudioTrackManagerObserverInterface>& observer) = 0;

    /**
     * Remove an observer from the focus manager.
     *
     * @param observer The observer to remove.
     */
    virtual void removeObserver(
        const std::shared_ptr<AudioTrackManagerObserverInterface>& observer) = 0;
};

}  // namespace channel
}  // namespace utils
}  // namespace aisdk

#endif  // _AUDIO_TRACK_MANAGER_INTERFACE_H_
