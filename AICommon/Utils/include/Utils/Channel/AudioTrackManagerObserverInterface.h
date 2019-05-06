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

#ifndef _AUDIO_TRACK_MANAGER_OBSERVER_INTERFACE_H_
#define _AUDIO_TRACK_MANAGER_OBSERVER_INTERFACE_H_

#include "Utils/Channel/FocusState.h"

namespace aisdk {
namespace utils {
namespace channel {

/// An interface that clients can extend to register to observe focus changes.  -- for Alarm
class AudioTrackManagerObserverInterface {
public:
    virtual ~AudioTrackManagerObserverInterface() = default;

    /**
     * Used to notify the observer of focus changes. This function should return quickly.
     *
     * @param channelName The name of the channel
     * @param newTrace The new @c AudioTrackState of @c channelName.
     */
    virtual void onTrackChanged(const std::string& channelName, utils::channel::FocusState newTrace) = 0;
};

}  // namespace channel
}  // namespace utils
}  // namespace aisdk

#endif  // _AUDIO_TRACK_MANAGER_OBSERVER_INTERFACE_H_