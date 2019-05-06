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
 
#ifndef _DIALOG_UXSTATE_RELAY_H_
#define _DIALOG_UXSTATE_RELAY_H_
#include <memory>
#include <unordered_set>

#include "Utils/DialogRelay/DialogUXStateObserverInterface.h"
#include "Utils/SoundAi/SoundAiObserverInterface.h"
#include "Utils/Threading/Executor.h"
#include "DMInterface/SpeechSynthesizerObserverInterface.h"

namespace aisdk {
namespace utils {
namespace dialogRelay {

/**
 * This class serves as a component to aggregate other(SoundAi\SpeechSynthesizer) observer interfaces into one UX component and
 * Users can get device interactive status through these component
 */
class DialogUXStateRelay
        : public soundai::SoundAiObserverInterface
        , public dmInterface::SpeechSynthesizerObserverInterface {
public:
    /**
     * Constructor.
     */
    DialogUXStateRelay();

    /**
     * Adds an observer to be notified of UX state changes.
     *
     * @param observer The new observer to notify of UX state changes.
     */
    void addObserver(std::shared_ptr<DialogUXStateObserverInterface> observer);

    /**
     * Removes an observer from the internal collection of observers synchronously. If the observer is not present,
     * nothing will happen.
     *
     * @param observer The observer to remove.
     */
    void removeObserver(std::shared_ptr<DialogUXStateObserverInterface> observer);
	
	/// @ name SoundAiObserverInterface method;
	/// @{
    void onStateChanged(soundai::SoundAiObserverInterface::State state) override;
	void onKeyWordDetected(std::string dialogId, std::string keyword, float angle) override;
	/// @}
	
	/// @name SpeechSynthesizerObserverInterface method;
	/// @{
    void onStateChanged(dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState state) override;
	/// @}
private:
	/**
     * Sets the internal state to @c IDLE if both @c SpeechSynthesizer and @c GenericAutomaticSpeechRecognizer are in idle state.
     */
	void tryEnterIdleState();
    /**
     * Notifies all observers of the current state. This should only be used within the internal executor.
     */
    void notifyObserversOfState();

    /**
     * Sets the internal state to the new state.
     *
     * @param newState The new UX state.
     */
    void setState(dialogRelay::DialogUXStateObserverInterface::DialogUXState newState);

    /// @{

    /// The @c UXObserverInterface to notify UX state.
    std::unordered_set<std::shared_ptr<dialogRelay::DialogUXStateObserverInterface>> m_observers;

	/// An internal executor
	threading::Executor m_executor;
	
    /// The current overall UX state.
    dialogRelay::DialogUXStateObserverInterface::DialogUXState m_currentState;

    /// Contains the current state of the @c SoundAi as reported by @c SoundAiObserverInterface
    soundai::SoundAiObserverInterface::State m_soundAiState;

	/// Contains the current state of the @c SpeechSynthesizer as reported by @c SpeechSynthesizerObserverInterface
    dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState m_speechSynthesizerState;
};

}	// namespace dialogRelay
}   // namespace utils
}   // namespace aisdk

#endif 	//_DIALOG_UXSTATE_RELAY_H_
