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

#ifndef __IFLYTEK_KEYWORD_DETECTOR_H_
#define __IFLYTEK_KEYWORD_DETECTOR_H_

#include <memory>
#include <thread>
#include <atomic>

#include "DMInterface/KeyWordObserverInterface.h"
#include "KWD/GenericKeywordDetector.h"
//#include "Recorder/CaptureEngine.h"

namespace aisdk {
namespace kwd {

// A  class that creates a instance keyword detector of @c IflyTekKeywordDetector.
class IflyTekKeywordDetector : public GenericKeywordDetector {
public:
	/**
     * Creates a @c IflyTekKeywordDetector.
     *
     * @param stream The stream of audio data.
     * @param keyWordObservers The observers to notify of keyword detections.
     * @Return A new @c IflyTekKeywordDetector, or @c nullptr if the operation failed.
     */
	static std::unique_ptr<IflyTekKeywordDetector> create(
		std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
		std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>> keywordObserver,
		std::chrono::milliseconds maxSamplesPerPush = std::chrono::milliseconds(10));

	bool run();

	~IflyTekKeywordDetector();
	
private:
	/**
	 * Constructor.
	 */
	IflyTekKeywordDetector(
		std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
		std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>> keywordObserver,
		std::chrono::milliseconds maxSamplesPerPush);

	void detectionLoop();

	static int keyWordDetectedCallback(
		const char *sessionID, 
		int msg,
		int param1,
		int param2,
		const void *info,
		void *userData);
	
	bool init();

	std::atomic<bool> m_isShuttingDown;

    /// The stream of audio data.
    const std::shared_ptr<utils::sharedbuffer::SharedBuffer> m_stream;

    /// The reader that will be used to read audio data from the stream.
    std::shared_ptr<utils::sharedbuffer::Reader> m_streamReader;

	/// Internal thread that reads audio from the buffer and feeds it to the Sensory engine.
	std::thread m_detectionThread;

	/// The IVW MSC handle
	std::string m_sessionId;

    /**
     * The max number of samples to push into the MSC engine per read data from @c m_streamReader.
     * This will be determined based on the sampling rate of the audio data passed in.
     */
    const size_t m_maxSamplesPerPush;	
};

}
}
#endif // __IFLYTEK_KEYWORD_DETECTOR_H_