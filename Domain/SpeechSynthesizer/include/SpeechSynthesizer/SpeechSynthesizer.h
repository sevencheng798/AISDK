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

 #ifndef __SPEECH_SYNTHESIZER_H_
 #define __SPEECH_SYNTHESIZER_H_
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <deque>

#include <Utils/Channel/ChannelObserverInterface.h>
#include <Utils/Channel/AudioTrackManagerInterface.h>
#include <Utils/DialogRelay/DialogUXStateObserverInterface.h>
#include <Utils/DialogRelay/DialogUXStateRelay.h>
#include <Utils/MediaPlayer/MediaPlayerInterface.h>
#include <Utils/MediaPlayer/MediaPlayerObserverInterface.h>
#include <Utils/SafeShutdown.h>
#include <DMInterface/SpeechSynthesizerObserverInterface.h>
#include <NLP/DomainProxy.h>

namespace aisdk {
namespace domain {
namespace speechSynthesizer {

/*
 * This class implements the SpeechSynthesizer for real-time conversation domains.
 * The domain include (weather\time\huangli\cookbook\baike\calculator\poem\chat\forex\stock)
 */
 //public nlp::DomainProxy,
class SpeechSynthesizer
		: public nlp::DomainProxy
		, public utils::SafeShutdown
		, public utils::mediaPlayer::MediaPlayerObserverInterface
		, public utils::dialogRelay::DialogUXStateObserverInterface		
		, public std::enable_shared_from_this<SpeechSynthesizer> {
public:
	
    /**
     * Create a new @c SpeechSynthesizer instance.
     *
     * @param mediaPlayer The instance of the @c MediaPlayerInterface used to play audio.
     * @param trackManager The instance of the @c AudioTrackManagerInterface used to acquire track of a channel.
     * @param dialogUXStateRelay The instance of the @c DialogUXStateRelay to use to notify user device interactive state.
     *
     * @return Returns a new @c SpeechSynthesizer, or @c nullptr if the operation failed.
     */
    static std::shared_ptr<SpeechSynthesizer> create(
        std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> mediaPlayer,
        std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager,
        std::shared_ptr<utils::dialogRelay::DialogUXStateRelay> dialogUXStateRelay);

	/// @name DomainProxy method.
	/// @{
	/// DomainHandlerInterface we need to definition the base class pure-virturl functions.
	void onDeregistered() override;
	
	void preHandleDirective(std::shared_ptr<DirectiveInfo> info) override;

	void handleDirective(std::shared_ptr<DirectiveInfo> info) override;

	void cancelDirective(std::shared_ptr<DirectiveInfo> info) override;

	/// @}
	
	/// @name ChannelObserverInterface method.
	/// @{
	void onTrackChanged(utils::channel::FocusState newTrace) override;
	/// @}
	
	/// @name MediaPlayerObserverInterface method.
	/// @{
	void onPlaybackStarted(SourceId id) override;
	
	void onPlaybackFinished(SourceId id) override;
	
	void onPlaybackError(SourceId id, const utils::mediaPlayer::ErrorType& type, std::string error) override;
	
	void onPlaybackStopped(SourceId id) override;
	 /// @}

	/// @name DialogUXStateObserverInterface method.
	/// @{
	 void onDialogUXStateChanged(DialogUXState newState) override;
	/// @}

	/*
	 * Add an observer to SpeechSynthesizer.
	 * 
	 * @param observer The observer to add.
	 */
	void addObserver(std::shared_ptr<dmInterface::SpeechSynthesizerObserverInterface> observer);

	/// Remove an observer from the SpeechSynthesizer.
	/// @param observer The ovserver to remove.
	void removeObserver(std::shared_ptr<dmInterface::SpeechSynthesizerObserverInterface> observer);

	/// Get the name of the execution DomainHandler. 
	std::string getHandlerName() const override;
	
protected:
	
	/// @name SafeShutdown method.
	/// @{
	void doShutdown() override;
	/// @}
	
private:

    /**
     * This class has all the data that is needed to process @c Chat(url_tts) directives.
     */
    class ChatDirectiveInfo {
    public:
		
        /**
         * Constructor.
         *
         * @param directiveInfo The @c DirectiveInfo.
         */
		ChatDirectiveInfo(std::shared_ptr<DirectiveInfo> directiveInfo);

		/// Release Chat specific resources.
        void clear();

        /// The Domain directive @c NLPDomain that is passed during preHandle.
        std::shared_ptr<nlp::NLPDomain> directive;

        /// @c DomainHandlerResultInterface.
        std::shared_ptr<dmInterface::DomainHandlerResultInterface> result;
		
		/// Identifies the location of audio content.  the value will be a remote http/https location.
		/// the variable will be removed.
		std::string url;
		/// The attachment reader @c AttachmentReader to read speech audio('text to speech').
		std::unique_ptr<utils::attachment::AttachmentReader> attachmentReader;
		
        /// A flag to indicate if the domain directive complete message has to be sent to the @c DomainSequencer.
        bool sendCompletedMessage;
    };

	/**
	 * Constructor
	 *
	 * @param mediaPlayer The instance of the @c MediaPlayerInterface used to play audio.
	 * @param trackManager The instance of the @c FocusManagerInterface used to acquire focus of a channel.
	 */
	SpeechSynthesizer(
		std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> mediaPlayer,
		std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager);

    /**
     * Initializes the @c SpeechSynthesizer.
     * Adds the @c SpeechSynthesizer as an observer of the speech player.
     */
    void init();

    /**
     * Pre-handle a SpeechSynthesizer.Chat directive (on the @c m_executor threadpool) to parse own keys and values.
     *
     * @param info The directive to pre-handle and the associated data.
     */
    void executePreHandleAfterValidation(std::shared_ptr<ChatDirectiveInfo> info);

    /**
     * Handle a SpeechSynthesizer.Chat directive (on the @c m_executor threadpool).  This starts a request for
     * the foreground trace focus.
     *
     * @param info The directive to handle and the associated data.
     */
    void executeHandleAfterValidation(std::shared_ptr<ChatDirectiveInfo> info);

    /**
     * Pre-handle a SpeechSynthesizer.Chat directive (on the @c m_executor thread). This starts a request parse.
     *
     * @param info The directive to preHandle
     */
    void executePreHandle(std::shared_ptr<DirectiveInfo> info);

    /**
     * Handle a SpeechSynthesizer.Chat directive (on the @c m_executor threadpool).  This starts a request for
     * the foreground trace focus.
     *
     * @param info The directive to handle and the result object with which to communicate the result.
     */
    void executeHandle(std::shared_ptr<DirectiveInfo> info);

    /**
     * Cancel execution of a SpeechSynthesizer.Chat directive (on the @c m_executor threadpool).
     *
     * @param info The directive to cancel.
     */
    void executeCancel(std::shared_ptr<DirectiveInfo> info);

    /**
     * Execute a change of state (on the @c m_executor threadpool). If the @c m_desiredState is @c PLAYING, playing the
     * audio of the current directive is started. If the @c m_desiredState is @c FINISHED this method triggers
     * termination of playing the audio.
     */
    void executeStateChange();

    /**
     * Handle (on the @c m_executor threadpool) notification that speech playback has started.
     */
    void executePlaybackStarted();

    /**
     * Handle (on the @c m_executor threadpool) notification that speech playback has finished.
     */
    void executePlaybackFinished();

    /**
     * Handle (on the @c m_executor threadpool) notification that speech playback encountered an error.
     *
     * @param error Text describing the nature of the error.
     */
    void executePlaybackError(const utils::mediaPlayer::ErrorType& type, std::string error);

    /**
     * Start playing speech audio.
     */
    void startPlaying();

    /**
     * Stop playing speech audio.
     */
    void stopPlaying();

	/**
     * Set the current state of the @c SpeechSynthesizer.
     * @c m_mutex must be acquired before calling this function. All observers will be notified of a new state
     *
     * @param newState The new state of the @c SpeechSynthesizer.
     */
    void setCurrentStateLocked(
    	dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState newState);
		
    /**
     * Set the desired state the @c SpeechSynthesizer needs to transition to based on the @c newTrace.
     * @c m_mutex must be acquired before calling this function.
     *
     * @param newTrace The new track focus of the @c SpeechSynthesizer.
     */
    void setDesiredStateLocked(utils::channel::FocusState newTrace);

	/**
     * Reset @c m_currentInfo, cleaning up any @c SpeechSynthesizer resources and removing from DomainProxy's
     * map of active @c NLPDomain.
     *
     * @param info The @c DirectiveInfo instance to make current (defaults to @c nullptr).
     */
	void resetCurrentInfo(std::shared_ptr<ChatDirectiveInfo> info = nullptr);

    /**
     * Send the handling completed notification to @c domainRouter and clean up the resources of @c m_currentInfo.
     */
	void setHandlingCompleted();

    /**
     * Report a failure to handle the @c NLPDomain.
     *
     * @param info The @c NLPDomain that encountered the error and ancillary information.
     * @param message The error message to include in the ExceptionEncountered message.
     */
	void reportExceptionFailed(
		std::shared_ptr<ChatDirectiveInfo> info,
        const std::string& message);

    /**
     * Release the @c FOREGROUND trace focus (if we have it).
     */
    void releaseForegroundTrace();

    /**
     * Reset the @c m_mediaSourceId once the @c MediaPlayer finishes with playback or
     * when @c MediaPlayer returns an error.
     */
    void resetMediaSourceId();

    /**
     * Verify a pointer to a well formed @c ChatDirectiveInfo.
     *
     * @param caller Name of the method making the call, for logging.
     * @param info The @c DirectiveInfo to test.
     * @param checkResult Check if the @c DomainHandlerResultInterface is not a @c nullptr in the @c DirectiveInfo.
     * @return A @c ChatDirectiveInfo if it is well formed, otherwise @c nullptr.
     */
    std::shared_ptr<ChatDirectiveInfo> validateInfo(
        const std::string& caller,
        std::shared_ptr<DirectiveInfo> info,
        bool checkResult = true);

    /**
     * Find the @c ChatDirectiveInfo instance (if any) for the specified @c messageId.
     *
     * @param messageId The messageId value to find @c ChatDirectiveInfo instance.
     * @return The @c ChatDirectiveInfo instance for @c messageId.
     */
    std::shared_ptr<ChatDirectiveInfo> getChatDirectiveInfo(const std::string& messageId);

    /**
     * Checks if the @c messageId is already present in the map. If its not present, adds an entry to the map.
     *
     * @param messageId The @c messageId value to add to the @c m_chatDirectiveInfoMap
     * @param info The @c ChatDirectiveInfo corresponding to the @c messageId to add.
     * @return @c true if @c messageId to @c ChatDirectiveInfo mapping was added. @c false if entry already exists
     * for the @c messageId.
     */
    bool setChatDirectiveInfo(
        const std::string& messageId,
        std::shared_ptr<SpeechSynthesizer::ChatDirectiveInfo> info);

    /**
     * Adds the @c ChatDirectiveInfo to the @c m_chatInfoQueue.
     *
     * @param info The @c ChatDirectiveInfo to add to the @c m_chatInfoQueue.
     */
    void addToDirectiveQueue(std::shared_ptr<ChatDirectiveInfo> info);

    /**
     * Removes the @c ChatDirectiveInfo corresponding to the @c messageId from the @c m_chatDirectiveInfoMap.
     *
     * @param messageId The @c messageId to remove.
     */
    void removeChatDirectiveInfo(const std::string& messageId);	

    /**
     * This function is called whenever the NLP UX dialog state of the system changes.
     *
     * @param newState The new dialog specific NLP UX state.
     */
    void executeOnDialogUXStateChanged(
        utils::dialogRelay::DialogUXStateObserverInterface::DialogUXState newState);

	/// The name of DomainHandler identifies which @c DomainHandlerInterface operates on.
	std::string m_handlerName;
	
	/// MediaPlayerInterface instance to send tts audio to MediaPlayer interface and playback.
	std::shared_ptr<utils::mediaPlayer::MediaPlayerInterface> m_speechPlayer;

	/// AudioTrackManagerInterface instance to acqurie the channel.
	std::shared_ptr<utils::channel::AudioTrackManagerInterface> m_trackManager;

    /// Id to identify the specific source when making calls to MediaPlayerInterface.
    utils::mediaPlayer::MediaPlayerInterface::SourceId m_mediaSourceId;
	
    /// The set of @c SpeechSynthesizerObserverInterface instances to notify of state changes.
    std::unordered_set<std::shared_ptr<dmInterface::SpeechSynthesizerObserverInterface>> m_observers;

    /**
     * The current state of the @c SpeechSynthesizer. @c m_mutex must be acquired before reading or writing the
     * @c m_currentState.
     */
    dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState m_currentState;

    /**
     * The state the @c SpeechSynthesizer must transition to. @c m_mutex must be acquired before reading or writing
     * the @c m_desiredState.
     */
    dmInterface::SpeechSynthesizerObserverInterface::SpeechSynthesizerState m_desiredState;

    /// The current trace acquired by the @c SpeechSynthesizer.
    utils::channel::FocusState m_currentFocus;

	/// @c ChatDirectiveInfo instance for the @c NLPDomain currently being handled.
	std::shared_ptr<ChatDirectiveInfo> m_currentInfo;

	/// Mutex to serialize access to m_currentState, m_desiredState, and m_waitOnStateChange.
	std::mutex m_mutex;

	/// A flag to keep track of if @c SpeechSynthesizer has called @c Stop() already or not.
	bool m_isAlreadyStopping;

	/// Condition variable to wake @c onFocusChanged() once the state transition to desired state is complete.
	std::condition_variable m_waitOnStateChange;
	
    /// Map of message Id to @c ChatDirectiveInfo.
    std::unordered_map<std::string, std::shared_ptr<ChatDirectiveInfo>> m_chatDirectiveInfoMap;

    /**
     * Mutex to protect @c messageId to @c ChatDirectiveInfo mapping. This mutex must be acquired before accessing
     * or modifying the m_chatDirectiveInfoMap
     */
    std::mutex m_chatDirectiveInfoMutex;

    /// Queue which holds the directives to be processed.
    std::deque<std::shared_ptr<ChatDirectiveInfo>> m_chatInfoQueue;

    /// Serializes access to @c m_chatInfoQueue
    std::mutex m_chatInfoQueueMutex;

	/// An internal thread pool which queues up operations from asynchronous API calls
	utils::threading::Executor m_executor;

};

}	// namespace speechSynthesizer
} 	//namespace domain
}	//namespace aisdk

#endif 	//__SPEECH_SYNTHESIZER_H_
