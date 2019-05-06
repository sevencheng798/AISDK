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

#include "KWD/KeywordDetectorRegister.h"
#if defined(KWD_SOUNDAI)

#elif defined(KWD_IFLYTEK)
#include "IflyTekKeywordDetector.h"
#endif

namespace aisdk {
namespace kwd {

using namespace utils::sharedbuffer;
using namespace dmInterface;

/// String to identify log entries originating from this file.
static const std::string TAG("KeywordDetectorRegister");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) utils::logging::LogEntry(TAG, event)

std::unique_ptr<GenericKeywordDetector> KeywordDetectorRegister::create(
std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
		std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>> keyWordObserver) {
	if(!stream) {
		AISDK_ERROR(LX("CreateFailed").d("reason", "SDSIsnullptr"));
		return nullptr;
	}

#if defined(KWD_SOUNDAI)
	#error "lkkkttt"
#elif defined(KWD_IFLYTEK)

	return IflyTekKeywordDetector::create(stream, keyWordObserver);
#else
	#error "No set any keyword detector."
	return nullptr;
#endif
}

}  // namespace kwd
}  // namespace alexaClientSDK
