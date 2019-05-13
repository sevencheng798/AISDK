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

#ifndef __AUTOMATIC_SPEECH_RECOGNIZER_REGISTER_H_
#define __AUTOMATIC_SPEECH_RECOGNIZER_REGISTER_H_

#include <unordered_set>

#include <DMInterface/MessageConsumerInterface.h>
#include <Utils/DeviceInfo.h>
#include <Utils/SharedBuffer/BufferLayout.h>
#include <Utils/Attachment/AttachmentManagerInterface.h>
#include <Utils/Channel/AudioTrackManagerInterface.h>
#include <Utils/SoundAi/SoundAiObserverInterface.h>
#include "ASR/GenericAutomaticSpeechRecognizer.h"
#include "ASR/AutomaticSpeechRecognizerConfiguration.h"

namespace aisdk {
namespace asr {
// A class that create a new ASR @c AutomaticSpeechRecognizer.
class AutomaticSpeechRecognizerRegister {
public:
 	/**
	 * Creates a @c SpeechRecognizer.
	 *
     * @param stream The stream of audio data. This should be formatted:
	 * - 16bit Linear PCM
	 * - 16kHz sample rate
	 * - 1 channels
     * @param asrObserver The observers to notify of keyword detections.
     *
     * @return An @c GenericAutomaticSpeechRecognizer if creation success or @c nullptr if creation failed.
     *
     * @Note: Return a point @c shared_ptr instance in here because enable_shared_from_this was called in subsequent classes.
	 */
	static std::shared_ptr<GenericAutomaticSpeechRecognizer> create(
		const std::shared_ptr<utils::DeviceInfo>& deviceInfo,
		std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager,
		std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker,
		std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
		const AutomaticSpeechRecognizerConfiguration& config = AutomaticSpeechRecognizerConfiguration());

};

}  // namespace asr
}  // namespace aisdk

#endif  // __AUTOMATIC_SPEECH_RECOGNIZER_REGISTER_H_