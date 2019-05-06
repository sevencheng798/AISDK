/*
 * Copyright 2019 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef __CONTROL_ACTION_MANAGER_H_
#define __CONTROL_ACTION_MANAGER_H_

#include <memory>

#include <Utils/Microphone/MicrophoneInterface.h>
#include <Utils/SafeShutdown.h>

#include "Application/AIClient.h"
#include "KeywordObserver.h"
#include "UIManager.h"

namespace aisdk {
namespace application {

/**
 * This class manages most of the user control interaction by taking in commands and notifying the AIClient and the
 * userInterface (the view) accordingly.
 */
class ControlActionManager
        : public utils::SafeShutdown {
public:
    /**
     * Constructor.
     */
    ControlActionManager(
        std::shared_ptr<AIClient> client,
        std::shared_ptr<utils::microphone::MicrophoneInterface> micWrapper,
        std::shared_ptr<UIManager> userInterface);

    /**
     * Begins the interaction between the Sample App and the user. This should only be called at startup.
     */
    void begin();

    /**
     * Should be called when a user requests help.
     */
    void help();

    /**
     * Toggles the microphone state if the Sample App was built with wakeword. When the microphone is turned off, the
     * app enters a privacy mode in which it stops recording audio data from the microphone, thus disabling Alexa waking
     * up due to wake word. Note however that hold-to-talk and tap-to-talk modes will still work by recording
     * microphone data temporarily until a user initiated interacion is complete. If this app was built without wakeword
     * then this will do nothing as the microphone is already off.
     */
    void microphoneToggle();

    /**
     * Should be called whenever a user presses and releases the tap button.
     */
    void tap();

    /**
     * Should be called whenever a user presses invalid option.
     */
    void errorValue();

    /**
     * Reset the device and remove any customer data.
     */
    void resetDevice();
private:
    /// The default SDK client.
    std::shared_ptr<AIClient> m_client;

    /// The microphone managing object.
    std::shared_ptr<utils::microphone::MicrophoneInterface> m_micWrapper;

    /// The user interface manager.
    std::shared_ptr<UIManager> m_userInterface;
	
    /// Whether the microphone is currently turned on.
    bool m_isMicOn;

    /**
     * An internal executor that performs execution of callable objects passed to it sequentially but asynchronously.
     */
    utils::threading::Executor m_executor;

    /// @name RequiresShutdown Functions
    /// @{
    void doShutdown() override;
    /// @}
};

}  // namespace application
}  // namespace aisdk

#endif  // __CONTROL_ACTION_MANAGER_H_