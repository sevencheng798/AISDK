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

#ifndef _AUDIO_TRACE_MANAGER_H_
#define _AUDIO_TRACE_MANAGER_H_

#include <mutex>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <Utils/Channel/ChannelObserverInterface.h>
#include <Utils/Channel/AudioTrackManagerInterface.h>

#include "AudioTrackManager/Channel.h"
#include "Utils/Threading/Executor.h"

namespace aisdk {
namespace atm {

/**
 * The AudioTrackManager takes requests to acquire and release Channels and updates the traces of other Channels based on
 * their priorities so that the invariant that there can only be one Foreground Channel is held. The following
 * operations are provided:
 */
class AudioTrackManager : public utils::channel::AudioTrackManagerInterface {
public:
    /**
     * The configuration used by the AudioTrackManager to create Channel objects. Each configuration object has a
     * name and priority.
     */
    struct ChannelConfiguration {
        /**
         * Constructor.
         *
         * @param configName The name of the Channel.
         * @param configPriority The priority of the channel. Lower number priorities result in higher priority
         * Channels. The highest priority number possible is 0.
         */
        ChannelConfiguration(const std::string& configName, unsigned int configPriority):
                name{configName},
                priority{configPriority} {
        }

        /**
         * Converts the config to a string.
         *
         * @return A string version of the @c ChannelConfiguration data.
         */
        std::string toString() const {
            return "name:'" + name + "', priority:" + std::to_string(priority);
        }

        /// The name of the channel.
        std::string name;

        /// The priority of the channel.
        unsigned int priority;
    };

    /**
     * This constructor creates Channels based on the provided configurations.
     */
    AudioTrackManager(
        const std::vector<ChannelConfiguration> channelConfigurations = {
            {DIALOG_CHANNEL_NAME, DIALOG_CHANNEL_PRIORITY}, 
            {ALARMS_CHANNEL_NAME, ALARMS_CHANNEL_PRIORITY}, 
            {MEDIA_CHANNEL_NAME, MEDIA_CHANNEL_PRIORITY}
        });

	/// name AudioTrackManagerInterface method:
	/// @{
    bool acquireChannel(
        const std::string& channelName,
        std::shared_ptr<utils::channel::ChannelObserverInterface> channelObserver,
        const std::string& interface) override;

    std::future<bool> releaseChannel(
        const std::string& channelName,
        std::shared_ptr<utils::channel::ChannelObserverInterface> channelObserver) override;

    void stopForegroundActivity() override;

    void addObserver(const std::shared_ptr<utils::channel::AudioTrackManagerObserverInterface>& observer) override;

    void removeObserver(
        const std::shared_ptr<utils::channel::AudioTrackManagerObserverInterface>& observer) override;
	/// @}
	
private:
    /**
     * Functor so that we can compare Channel objects via shared_ptr.
     */
    struct ChannelPtrComparator {
        /**
         * Utility two-place predicate - Compares two shared_ptrs of Channel objects.
         *
         * @param first The first Channel to compare.
         * @param second The second Channel to compare.
         *
         * @return Returns @c true if the first Channel has a higher Channel priority than the second Channel.
         */
        bool operator()(const std::shared_ptr<Channel>& first, const std::shared_ptr<Channel>& second) const {
            return *first > *second;
        }
    };

    /**
     * Sets the @c AudioTrackState for @c channel and notifies observers of the change.
     *
     * @param channel The @c Channel to set the @c FocusState for.
     * @param trace The @c AudioTrackState to set @c channel to.
     */
    void setChannelTrack(const std::shared_ptr<Channel>& channel, utils::channel::FocusState trace);

    /**
     * Grants access to the Channel specified and updates other Channels as needed. This function provides the full
     * implementation which the public method will call.
     *
     * @param channelToAcquire The Channel to acquire.
     * @param channelObserver The new observer of the Channel.
     */
    void acquireChannelHelper(
        std::shared_ptr<Channel> channelToAcquire,
        std::shared_ptr<utils::channel::ChannelObserverInterface> channelObserver,
        const std::string &interface);

    /**
     * Releases the Channel specified and updates other Channels as needed. This function provides the full
     * implementation which the public method will call.
     *
     * @param channelToRelease The Channel to release.
     * @param channelObserver The observer of the Channel to release.
     * @param releaseChannelSuccess The promise to satisfy.
     * @param channelName The name of the Channel.
     */
    void releaseChannelHelper(
        std::shared_ptr<Channel> channelToRelease,
        std::shared_ptr<utils::channel::ChannelObserverInterface> channelObserver,
        std::shared_ptr<std::promise<bool>> releaseChannelSuccess,
        const std::string& channelName);

    /**
     * Stops the Channel specified and updates other Channels as needed if the interface name passed in matches the
     * interface occupying the Channel. This function provides the full implementation which the public method will
     * call.
     *
     * @param foregroundChannel The Channel to stop.
     * @param foregroundChannelInterface The name of the interface to stop.
     */
    void stopForegroundActivityHelper(
        std::shared_ptr<Channel> foregroundChannel,
        std::string foregroundChannelInterface);

    /**
     * Finds the channel from the given channel name.
     *
     * @param channelName The name of the channel to find.
     * @return Return a @c Channel if found or @c nullptr otherwise.
     */
    std::shared_ptr<Channel> getChannel(const std::string& channelName) const;

    /**
     * Gets the currently foregrounded Channel.
     *
     * @return Returns the highest priority active Channel if there is one and @c nullptr otherwise.
     */
    std::shared_ptr<Channel> getHighestPriorityActiveChannelLocked() const;

    /**
     * Checks to see if the provided channel currently has foreground focus or not.
     *
     * @param channel The channel to check.
     * @return Returns @c true if the @c Channel is foreground focused and @c false if it is not.
     */
    bool isChannelForegroundedLocked(const std::shared_ptr<Channel>& channel) const;

    /**
     * Checks to see if the provided Channel name already exists.
     *
     * @param name The Channel name to check.
     *
     * @return Returns @c true if the name is already associated with a Channel and @c false otherwise.
     */
    bool doesChannelNameExist(const std::string& name) const;

    /**
     * Checks to see if the provided Channel priority already exists.
     *
     * @param priority The Channel priority to check.
     *
     * @return Returns @c true if the priority is already associated with a Channel and @c false otherwise.
     */
    bool doesChannelPriorityExist(const unsigned int priority) const;

    /**
     * Foregrounds the highest priority active Channel.
     */
    void foregroundHighestPriorityActiveChannel();

    /// Map of channel names to shared_ptrs of Channel objects and contains every channel.
    std::unordered_map<std::string, std::shared_ptr<Channel>> m_allChannels;

    /// Set of currently observed Channels ordered by Channel priority.
    std::set<std::shared_ptr<Channel>, ChannelPtrComparator> m_activeChannels;

    /// The set of observers to notify about track changes.
    std::unordered_set<std::shared_ptr<utils::channel::AudioTrackManagerObserverInterface>> m_observers;

    /// Mutex used to lock m_activeChannels, m_observers and Channels' interface name.
    std::mutex m_mutex;

	/// An internal thread pool.
    utils::threading::Executor m_executor;
};

}  // namespace atm
}  // namespace aisdk

#endif  // _AUDIO_TRACE_MANAGER_H_
