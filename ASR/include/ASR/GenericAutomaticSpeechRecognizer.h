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

#ifndef __GENERIC_AUTOMATIC_SPEECH_RECOGNIZER_H_
#define __GENERIC_AUTOMATIC_SPEECH_RECOGNIZER_H_

#include <mutex>
#include <future>
#include <unordered_set>

#include <Utils/SafeShutdown.h>
#include <Utils/SharedBuffer/SharedBuffer.h>
#include <Utils/Attachment/AttachmentManagerInterface.h>
#include <Utils/SoundAi/SoundAiObserverInterface.h>

namespace aisdk {
namespace asr {
// A class that create a new ASR @c AutomaticSpeechRecognizer.
class GenericAutomaticSpeechRecognizer : public utils::SafeShutdown {
public:
	/// A reserved @c Index value which is considered invalid.
    static const auto INVALID_INDEX = std::numeric_limits<utils::sharedbuffer::SharedBuffer::Index>::max();
    /**
     * Adds the specified observer to the list of observers to notify of ASR events.
     * @param asrObserver The observer to add.
     */
    void addASRObserver(std::shared_ptr<utils::soundai::SoundAiObserverInterface> asrObserver);

    /**
     * Removes the specified observer to the list of observers to notify of ASR events.
     * @param asrObserver The observer to remove.
     */
    void removeASRObserver(std::shared_ptr<utils::soundai::SoundAiObserverInterface> asrObserver);

	/**
	 * Start the ASR services. The current class nothing implementation and most cases are implemented
	 * in subclasses as much as possible.
	 * @return true if start sucessed, otherwise false.
	 */
	virtual bool start() = 0;
	/**
	 * Stop the ASR services. The current class nothing implementation and most cases are implemented
	 * in subclasses as much as possible.
	 * @return true if start sucessed, otherwise false.
	 */
	virtual bool stop() = 0;
	/**
	 * Reset the ASR services. The current class nothing implementation and most cases are implemented
	 * in subclasses as much as possible.
	 * @return true if start sucessed, otherwise false.
	 */
	virtual bool reset() = 0;
 	/**
     * This function asks the @c GenericAutomaticSpeechRecognizer to send a user utterance to NLP('AIUI' or 'Soundai Cloud')
     * and start streaming from @c SharedBuffer, which transitions it to the @c RECOGNIZING state. 
     *
     * @params stream The stream data @c SharedBuffer from MIC.
     * @params beginIndex The @c Index in @c SharedBuffer where audio streaming should begin. 
     * @params endIndex The text of the keyword which was recognized.
     */
	virtual std::future<bool> recognize(
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
    utils::sharedbuffer::SharedBuffer::Index begin = INVALID_INDEX,
    utils::sharedbuffer::SharedBuffer::Index keywordEnd = INVALID_INDEX) = 0;

	/**
	 * This function asks the ASR engine @c GenericAutomaticSpeechRecognizer to start a TTS requests.
	 * 
	 * @params text The text that will convert voice stream.
	 * @params writer The writer @c AttachmentWriter that receiving data stream after conversion to store.
	 * @return true if request sucess, otherwise return @c false.
	 */
	virtual std::future<bool> acquireTextToSpeech(
		const std::string text,
		std::shared_ptr<utils::attachment::AttachmentWriter> writer) = 0;

    /**
     * Destructor.
     */
    virtual ~GenericAutomaticSpeechRecognizer() = default;

protected:
    /**
     * Constructor.
     *
     * @param asrObservers The observers to notify of ASR.
     */
    GenericAutomaticSpeechRecognizer(
        std::unordered_set<std::shared_ptr<utils::soundai::SoundAiObserverInterface>> asrObservers =
            std::unordered_set<std::shared_ptr<utils::soundai::SoundAiObserverInterface>>());
#if 0
    /**
     * Notifies all keyword observers of the keyword detection.
     *
     * @param stream The stream in which the keyword was detected.
     * @param keyword The keyword detected.
     * @param beginIndex The absolute begin index of the first part of the keyword found within the @c stream.
     * @param endIndex The absolute end index of the last part of the keyword within the stream of the @c stream.
     * @param KWDMetadata Wake word engine metadata.
     */
    void notifyASRStateObservers(
        utils::soundai::SoundAiObserverInterface::State state) const;
#endif
    /**
     * Reads from the specified stream into the specified buffer and does the appropriate error checking and observer
     * notifications.
     *
     * @param reader The stream reader. This should be a blocking reader.
     * @param buf The buffer to read into.
     * @param nWords The number of words to read.
     * @param timeout The amount of time to wait for data to become available.
     * @param[out] errorOccurred Lets caller know if there were any errors that occurred with the read call.
     * @return The number of words successfully read.
     */
    ssize_t readFromStream(
        std::shared_ptr<utils::sharedbuffer::Reader> reader,
        void* buf,
        size_t nWords,
        std::chrono::milliseconds timeout,
        bool* errorOccurred);
	/**
     * This function updates the @c SoundAiObserverInterface state and notifies the state observer.  Any changes to
     * @c m_state should be made through this function.
     *
     * @param state The new state to change to.
     */
    void setState(utils::soundai::SoundAiObserverInterface::State state);

	/**
	 * This function get the @c SoundAiObserverInterface state.
	 * @return m_state value.
	 */
	utils::soundai::SoundAiObserverInterface::State getState() const;

	/// @name SafeShutdown method:
	/// @{
	void doShutdown() override;
	/// @}
	/**
	 * Close the instance of @c GenericAutomaticSpeechRecognizer. This function should ensure that subclass
	 * resources are recovered correctly.
	 */
	virtual void terminate() = 0;
	
private:
    /**
     * The observers to notify on key word detections. This should be locked with m_keyWordObserversMutex prior to
     * usage.
     */
    std::unordered_set<std::shared_ptr<utils::soundai::SoundAiObserverInterface>> m_asrObservers;

	/// The current state of the @c SoundAiObserverInterface.
    utils::soundai::SoundAiObserverInterface::State m_state;
	
    /// Lock to protect m_asrObservers when users wish to add or remove observers
    mutable std::mutex m_asrObserversMutex;

};

}  // namespace asr
}  // namespace aisdk

#endif  // __GENERIC_AUTOMATIC_SPEECH_RECOGNIZER_H_