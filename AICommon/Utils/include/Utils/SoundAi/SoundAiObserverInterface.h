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

#ifndef _SOUNDAIOBSERVERINTERFACE_H_
#define _SOUNDAIOBSERVERINTERFACE_H_

namespace aisdk { 
namespace utils {
namespace soundai {

/**
* A SoundAiObserverInterface is an interface class that clients can extend to listen for keyword detections and ai state capture.
*/
class SoundAiObserverInterface {
public:
	/// The different states the @c SoundAi can be in
    enum class State {
        /// In this state, the @c SoundAi is not waiting for or transmitting speech.
        IDLE,

        /// In this state, the @c SoundAi is waiting for a call to recognize().
        EXPECTING_SPEECH,

        /// In this state, the @c SoundAi is actively streaming speech.
        RECOGNIZING,

        /// In this state, the @c SoundAi has finished streaming and is waiting for completion of an Event.
        BUSY
    };
		
	/**
	* Destructor.
	*/
	virtual ~SoundAiObserverInterface() = default;

	/**
	* Used to notify the observer of keyword detections. 
	*
	* @param dialogId The stream in which the keyword was detected.
	* @param keyword The keyword detected.
	* @param angle Indicate wake-up location.
	*/
	virtual void onKeyWordDetected(
	 std::string dialogId,
	 std::string keyword,
	 float angle) = 0;

    /**
     * This function is called when the state of the observed @c sound ai changes. 
     *
     * @param state The new state of the @c SoundAiEngine.
     */
	virtual void onStateChanged(State state) = 0;

	/**
     * This function converts the provided @c State to a string.
     *
     * @param state The @c State to convert to a string.
     * @return The string conversion of @c state.
     */
    static std::string stateToString(State state) {
        switch (state) {
            case State::IDLE:
                return "IDLE";
            case State::EXPECTING_SPEECH:
                return "EXPECTING_SPEECH";
            case State::RECOGNIZING:
                return "RECOGNIZING";
            case State::BUSY:
                return "BUSY";
        }
        return "Unknown State";
    }
};

/**
 * Write an @c SoundAiObserverInterface::State value to an @c ostream as a string.
 *
 * @param stream The stream to write the value to.
 * @param state The @c SoundAiObserverInterface::State value to write to the @c ostream as a string.
 * @return The @c ostream that was passed in and written to.
 */
inline std::ostream& operator<<(std::ostream& stream, const SoundAiObserverInterface::State& state) {
    return stream << SoundAiObserverInterface::stateToString(state);
}

}	// namespace soundai
}	// namespace utils
} // namespace aisdk 
#endif  // _KEYWORDOBSERVERINTERFACE_H_

