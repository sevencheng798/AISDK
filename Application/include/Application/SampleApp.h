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

#ifndef __SAMPLE_APPLICATION_H_
#define __SAMPLE_APPLICATION_H_

#include <memory>
#include <AudioMediaPlayer/AOWrapper.h>
#include <KWD/GenericKeywordDetector.h>

#include "Application/AIClient.h"
#include "Application/UIManager.h"
#include "Application/ControlActionManager.h"

namespace aisdk {
namespace application {

/**
 * A class to manage the component of AIClient applicantion.
 */
class SampleApp {
public:
    /**
     * Constructor.
     *
     */
	static std::unique_ptr<SampleApp> createNew();

	/// Runs the application, blocking until the user asked app quit. 
	void run();

	/// Deconstructor.
	~SampleApp();

private:
	bool initialize();

	// The used to create libao objects.
	std::shared_ptr<mediaPlayer::ffmpeg::AOEngine> m_aoEngine;

	// The @c MediaPlayer used by @c SpeechSyth.
	std::shared_ptr<mediaPlayer::ffmpeg::AOWrapper> m_chatMediaPlayer;

	// The @c MediaPlayer used by @c Alarms.
	std::shared_ptr<mediaPlayer::ffmpeg::AOWrapper> m_streamMediaPlayer;

	// The @c MediaPlayer used by @c MediaStream.
	std::shared_ptr<mediaPlayer::ffmpeg::AOWrapper> m_alarmMediaPlayer;

	/// The default ai sdk client instance.
	std::shared_ptr<AIClient> m_aiClient;

	/// The Wakeword Detector which can wake up the client using audio input.
    std::unique_ptr<kwd::GenericKeywordDetector> m_keywordDetector;

	/// The instance of @c ControlActionManager to preform user request action.
	std::shared_ptr<ControlActionManager> m_controlActionManager;
};

}  // namespace application
}  // namespace aisdk

#endif  // __SAMPLE_APPLICATION_H_

