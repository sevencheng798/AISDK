/*
 * Copyright 2019 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef _ATM_CHANNEL_H_
#define _ATM_CHANNEL_H_

#include <chrono>
#include <memory>
#include <string>

#include <Utils/Channel/ChannelObserverInterface.h>
#include <Utils/Channel/FocusState.h>

namespace aisdk {
namespace atm {

/**
 * A Channel represents a audio track layer with a priority.
 */
class Channel {
public:
    /*
     * This class contains the states of the @c Channel.
     */
    struct State {
        /// Constructor with @c Channel name as the parameter.
        State(const std::string& name);

        /*
         * The channel's name.
         */
        std::string name;

        /// The current active audio track of the Channel.
        utils::channel::FocusState focusState;

		/// The name of the Audio Type interface that is occupying the Channel. - remove
        std::string interfaceName;

        /// Time at which the channel goes to NONE focus.
        std::chrono::steady_clock::time_point timeAtIdle;
    };

    /**
     * Constructs a new Channel object with the provided priority.
     *
     * @param name The channel's name.
     * @param priority The priority of the channel.
     */
    Channel(const std::string& name, const unsigned int priority);

    /**
     * Returns the name of a channel.
     *
     * @return The channel's name.
     */
    const std::string& getName() const;

    /**
     * Returns the priority of a Channel.
     *
     * @return The Channel priority.
     */
    unsigned int getPriority() const;

    /**
     * Updates the focus and notifies the Channel's observer, if there is one, of the focus change. The method mustbe wait
     * The ChannelObserverInterface##onTrackChanged() callback to the observer returns. 
     *
     * @param focus The focus of the Channel.
     * @return @c true if focus changed, else @c false.
     */
    bool setFocus(utils::channel::FocusState focus);

    /**
     * Sets a new observer.
     *
     * @param observer The observer of the Channel.
     */
    void setObserver(std::shared_ptr<utils::channel::ChannelObserverInterface> observer);

    /**
     * Checks whether the Channel has an observer.
     *
     * @return @c true if the Channel has an observer, else @c false.
     */
    bool hasObserver() const;

    /**
     * Checks which is higher priority.
     * @param rhs The Channel to compare with this Channel.
     */
    bool operator>(const Channel& rhs) const;

	/**
     * Updates the Audio type interface occupying the Channel.
     *
     * @param interface The name of the interface occupying the Channel.
     */
    void setInterface(const std::string& interface);

    /**
     * Returns the name of the Audio type interface occupying the Channel.
     *
     * @return The name of the Audio type interface.
     */
    std::string getInterface() const;

    /**
     * Checks whether the observer passed in currently owns the Channel.
     *
     * @param observer The observer to check.
     * @return @c true if the observer currently owns the Channel and @c false otherwise no found observer.
     */
    bool doesObserverOwnChannel(std::shared_ptr<utils::channel::ChannelObserverInterface> observer) const;

    /**
     * Returns the @c State of the @c Channel.
     *
     * @return The @c State.
     */
    Channel::State getState() const;

private:
    /// The priority of the Channel.
    const unsigned int m_priority;

    /// The @c State of the @c Channel.
    State m_state;

    /// The current observer of the Channel.
    std::shared_ptr<utils::channel::ChannelObserverInterface> m_observer;
};

}  // namespace atm
}  // namespace aisdk

#endif  // _ATM_CHANNEL_H_
