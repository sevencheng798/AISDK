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
#ifndef _DIALOGUXSTATE_OBSERVER_INTERFACE_H_
#define _DIALOGUXSTATE_OBSERVER_INTERFACE_H_

#include <string>

namespace aisdk {
namespace utils {
namespace dialogRelay {

/// A dialog specific UX state observer.
class DialogUXStateObserverInterface {
public:
    /// The different dialog specific NLP UX states.
    enum class DialogUXState {
        /// Device is idle and ready for an interaction.
        IDLE,

        /// Device is currently listening.
        LISTENING,

        /**
         * A customer request has been completed and no more input is accepted. In this state, device is waiting for a
         * response from NLP Cloud.
         * Note: this is a reserved state
         */
        THINKING,

        /// Device is responding to a request with speech.
        SPEAKING,

        /**
         * Device has finished processing a Domain(Dialog) directive.
         */
        FINISHED
    };

    /**
     * Destructor.
     */
    virtual ~DialogUXStateObserverInterface() = default;

    /**
     * This function is called whenever the device UX dialog state of the system changes.
     *
     * @param newState The new dialog specific NLP UX state.
     */
    virtual void onDialogUXStateChanged(DialogUXState newState) = 0;

    /**
     * This function converts the provided @c DialogUXState to a string.
     *
     * @param state The @c DialogUXState to convert to a string.
     * @return The string conversion of @c state.
     */
    static std::string stateToString(DialogUXState state);
};

inline std::string DialogUXStateObserverInterface::stateToString(DialogUXState state) {
    switch (state) {
        case DialogUXState::IDLE:
            return "IDLE";
        case DialogUXState::LISTENING:
            return "LISTENING";
        case DialogUXState::THINKING:
            return "THINKING";
        case DialogUXState::SPEAKING:
            return "SPEAKING";
        case DialogUXState::FINISHED:
            return "FINISHED";
    }
    return "Unknown State";
}

/**
 * Write a @c DialogUXStateObserverInterface::DialogUXState value to an @c ostream as a string.
 *
 * @param stream The stream to write the value to.
 * @param state The @c DialogUXStateObserverInterface::DialogUXState value to write to the @c ostream as a string.
 * @return The @c ostream that was passed in and written to.
 */
inline std::ostream& operator<<(std::ostream& stream, const DialogUXStateObserverInterface::DialogUXState& state) {
    return stream << DialogUXStateObserverInterface::stateToString(state);
}

}  // namespace dialogRelay
}  // namespace utils
}  // namespace aisdk

#endif  // _DIALOGUXSTATE_OBSERVER_INTERFACE_H_

