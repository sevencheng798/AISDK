/*
 * Copyright 2019 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef __AIUI_AUTOMATIC_SPEECH_RECOGNIZER_H_
#define __AIUI_AUTOMATIC_SPEECH_RECOGNIZER_H_

#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

#include "FileUtil.h"	// To support read data from file.

#include <DMInterface/MessageConsumerInterface.h>
#include <Utils/Threading/Executor.h>
#include <Utils/Channel/AudioTrackManagerInterface.h>
#include <Utils/Channel/ChannelObserverInterface.h>
#include "ASR/GenericAutomaticSpeechRecognizer.h"
#include "ASR/AutomaticSpeechRecognizerConfiguration.h"
#include "ASR/ASRTimer.h"
#include "AIUI/AIUIASRListener.h"
#include "AIUI/AIUIASRListenerObserverInterface.h"

namespace aisdk {
namespace asr {
namespace aiuiEngine {
// A class asr that iflytek aiui to implements the ASR function.
class AIUIAutomaticSpeechRecognizer
	: public utils::channel::ChannelObserverInterface
	, public GenericAutomaticSpeechRecognizer
	, public AIUIASRListenerObserverInterface
	, public AIUIASRListener
	, public std::enable_shared_from_this<AIUIAutomaticSpeechRecognizer> {
	
public:
	/// Values that express the result of receive a text to speech. 
	enum class TTSDataWriteStatus {
        /// The most recent chunk of data was received ok.
        OK,
        /// There was a problem handling the most recent chunk of data.
        ERROR		
	};
	
	/**
	 * Create a @c AIUIAutomaticSpeechRecognizer.
	 * @param config The configure params of @c AIUIAutomaticSpeechRecognizer.
	 *
	 * @return a @c AIUIAutomaticSpeechRecognizer, otherwise @c nullptr if creation failed.
	 */
	static std::shared_ptr<AIUIAutomaticSpeechRecognizer> create(
		std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager,
		std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker,
		std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
		const AutomaticSpeechRecognizerConfiguration& config);

	/// @name GenericAutomaticSpeechRecognizer method:
	/// @{
	bool start() override;
	bool stop() override;
	bool reset() override;
	std::future<bool> recognize(
		std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
        utils::sharedbuffer::SharedBuffer::Index begin = INVALID_INDEX,
        utils::sharedbuffer::SharedBuffer::Index keywordEnd = INVALID_INDEX) override;
	std::future<bool> acquireTextToSpeech(
		std::string text,
		std::shared_ptr<utils::attachment::AttachmentWriter> writer) override;
	/// }

	/// @name AIUIASRListener method:
	/// @{
	void handleEventStateReady() override;
	void handleEventStateWorking() override;
	void handleEventVadBegin() override;
	void handleEventResultNLP(const std::string unparsedIntent) override;
	void handleEventResultTPP(const std::string unparsedIntent) override;
	void handleEventResultTTS(const std::string info, const std::string data) override;
	void handleEventResultIAT() override;
	/// @}

	/// @name ChannelObserverInterface method.
	/// @{
	void onTrackChanged(utils::channel::FocusState newTrace) override;
	/// @}

	/**
     * A utility function to close the currently active attachment writer, if there is one.
     */
    void closeActiveAttachmentWriter();
	
	~AIUIAutomaticSpeechRecognizer();
protected:
	/// @name AutomaticSpeechRecognizer
	/// @{
	void terminate() override;
	/// @}
private:
	/**
	 * Constructor.
	 */
	AIUIAutomaticSpeechRecognizer(
		std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager,
		std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker,
		std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
		const std::string &appId,
		const std::string &aiuiConfigFile,
		const std::string &aiuiDir,
		const std::string &aiuiLogDir);

	bool init();

	/**
	 * The function to notify and destory tinkingTimer thread.
	 */
	// void resetThinkingTimeoutTimer();
	/**
	 * The function to implement notify a recgnize event and initiate acquire user speech to send.
	 *
	 * @params stream The stream data @c SharedBuffer from MIC.
	 * @params begin The @c Index in @c SharedBuffer where audio streaming should begin.
	 * @params keywordEnd The text of the keyword which was recognized.
	 * @return @c true if implement success, otherwise return @c false.
	 */
	bool executeRecognize(
		std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
        utils::sharedbuffer::SharedBuffer::Index begin,
        utils::sharedbuffer::SharedBuffer::Index keywordEnd);

	/**
	 * The function to implement text to speech.
	 * @params text The text content to be convert.
	 * @params writer The @c Writer used to writing stream stored to a sharedbuffer.
	 * @return true if success. otherwise @c false.
	 */
	bool executeTextToSpeech(
		const std::string text,
		std::shared_ptr<utils::attachment::AttachmentWriter> writer = nullptr);

	/**
	 * The function that receive respond data from IflyTek cloud.
	 * @params unparsedIntent The unparsed intent(answer) JSON string from AIUI Cloud.
	 *
	 * @return true if success. otherwise @c false.
	 */
	bool executeNLPResult(const std::string unparsedIntent);
	/**
	 * The function to implement text to speech.
	 * @params intent The unparsed intent(answer) JSON string from AIUI Cloud.
	 * @params writer The @c Writer used to writing stream stored to a sharedbuffer.
	 * @return true if success. otherwise @c false.
	 */
	bool executeTPPResult(
		const std::string intent,
		std::shared_ptr<utils::attachment::AttachmentWriter> writer = nullptr);
	/**
	 * The function that receive speech data after text conversion.
	 * @params info The info of @c onEvent() content from event.getInfo().
	 * @params data The speech data that converted text content.
	 * @return true if success. otherwise @c false.
	 */
	bool executeTTSResult(const std::string info, const std::string data);

    /**
     * This function is called when the @c AudioTrackManager trace changes.  This might occur when another component
     * acquires focus on the dialog channel, in which case the @c AIUIAutomaticSpeechRecognizer will end any activity and return
     * to @c IDLE. This function is also called after a call to @c executeRecognize() tries to acquire the channel.
     *
     * @param newTrace The track state to change to.
     */
    void executeOnTracKChanged(utils::channel::FocusState newTrace);
	
    /**
     * This function forces the @c AIUIAutimaticSpeechRecognizer back to the @c IDLE state.  This function 
     * can be called in any state, and will end any Event which is currently in progress.
     */
    void executeResetState();
	
	/**
	 * This function send user utterance to NLP Cloud(AIUI).
	 */
	void sendStreamProcessing();

    /**
     * Utility function to encapsulate the logic required to write data to an attachment.
     *
     * @param buffer The data to be written to the attachment.
     * @param size The size of the data to be written to the attachment.
     * @return A value expressing the final status @c TTSDataWriteStatus of the write operation.
     */
    TTSDataWriteStatus writeDataToAttachment(const char* buffer, size_t size);

	/**
	 *
	 */
	void transitionFromThinkingTimedOut();
	void transitionFromListeningTimedOut();

	/**
	 * The function that It must be ensured that there is a timeout mechanism when there
	 * is no response for a long time during the transition from recognition to thinking.
	 */
	void tryEntryIdleStateOnTimer();
	
	std::shared_ptr<utils::channel::AudioTrackManagerInterface> m_trackManager;

    /// The current track state of the @c AIUIAutomaticSpeechRecognizer on the dialog channel.
    utils::channel::FocusState m_trackState;

	/// A attachment docker is used to create @c AttachmentWriter and @c AttachmentReader.  
	std::shared_ptr<utils::attachment::AttachmentManagerInterface> m_attachmentDocker;
	
	/// A consumer which the semantics message has been receive from NLP Cloud('sai sdk' or 'Iflytek AIUI').
	std::shared_ptr<dmInterface::MessageConsumerInterface> m_messageConsumer;
		
	/// A unique ID from Iflytek.
	const std::string m_appId;

	/// Restore init default configure params file.
	const std::string m_aiuiConfigFile;

	/// Set the AIUI working directory, the SDK will save logs, data and other files in this path.
	const std::string m_aiuiDir;

	/// Initialize the logger and set the log save directory.
	const std::string m_aiuiLogDir;
	
	aiui::IAIUIAgent* m_aiuiAgent;

	std::atomic<bool> m_running;

	// The special sessionId for each interaction.
	std::string m_sessionId;

	std::shared_ptr<utils::sharedbuffer::Reader> m_reader;

	/// The current @c AttachmentWriter.
	std::shared_ptr<utils::attachment::AttachmentWriter> m_attachmentWriter;

	/// A timer to transition out of the LISTENING state.
	ASRTimer m_timeoutForListeningTimer;

	/// A timer to transition out of the THINKING state.
	ASRTimer m_timeoutForThinkingTimer;
	
	//FileUtil::DataFileHelper* mFileHelper;
	
	std::thread m_readerThread;

	utils::threading::Executor m_executor;
};

} // namespace aiui
} // namespace asr
} // namespace aisdk
#endif // __AIUI_AUTOMATIC_SPEECH_RECOGNIZER_H_