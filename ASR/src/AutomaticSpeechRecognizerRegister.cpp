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
#include <Utils/Logging/Logger.h>
#include "ASR/AutomaticSpeechRecognizerRegister.h"
#if defined(ENABLE_SOUNDAI_ASR)

#elif defined(ENABLE_IFLYTEK_AIUI_ASR)
#include "AIUI/AIUIAutomaticSpeechRecognizer.h"
#endif

namespace aisdk {
namespace asr {
/// String to identify log entries originating from this file.
static const std::string TAG("AutomaticSpeechRecognizerRegister");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) utils::logging::LogEntry(TAG, event)

std::shared_ptr<GenericAutomaticSpeechRecognizer> AutomaticSpeechRecognizerRegister::create(
	std::shared_ptr<utils::channel::AudioTrackManagerInterface> trackManager,
	std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker,
	std::shared_ptr<dmInterface::MessageConsumerInterface> messageConsumer,
	const AutomaticSpeechRecognizerConfiguration& config) {

#if defined(ENABLE_SOUNDAI_ASR)
	#error "You need to impletement soundai asr engine."
#elif defined(ENABLE_IFLYTEK_AIUI_ASR)
	return aiuiEngine::AIUIAutomaticSpeechRecognizer::create(trackManager, attachmentDocker, messageConsumer, config);
#else
	#error "No set any ASR enginer."
	return nullptr;
#endif

}

}  // namespace asr
}  // namespace aisdk