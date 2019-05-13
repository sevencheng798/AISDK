/*
 * Copyright 2019 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#ifndef __NET_EVENT_TYPES_H_
#define __NET_EVENT_TYPES_H_
#include <memory>

#include "sai_sdk.h"

namespace aisdk {
namespace asr {
namespace soundAiEngine {

/**
 * Convert an @c netEventType to an sai-compliant @c std::string.
 *
 * @param eventType The @c netEventType to convert.
 * @return The sai-compliant string representation of @c netEventType.
 */
inline std::string eventTypeToString(int eventType) {
	switch (eventType) {
		case SOUNDAI_FINAL_RESULT:
			return "ASR_EVT_FINAL_RESULT";
		case SOUNDAI_TEMP_RESULT:
			return "ASR_EVT_TEMP_RESULT";
		case SOUNDAI_ERROR:
			return "ASR_EVT_ERROR";
		case SOUNDAI_VAD:
			return "ASR_EVT_VAD";
		case SOUNDAI_SEMANTICS:
			return "ASR_EVT_SEMANTICS";
	}
	return "unknown netEventType";
}

/**
 * Convert an @c netEventType to an sai-compliant @c std::string.
 *
 * @param eventType The @c netEventType to convert.
 * @return The sai-compliant string representation of @c netEventType.
 */
inline std::string codeInfoToString(int rc) {
	switch (rc) {
		case SOUNDAI_ASR_ENGINE_ERROR:
			return "ASR_ENGINE_ERROR";
		case SOUNDAI_ALL:
			return "ASR_ALL_RESERVE";
		case SOUNDAI_OK:
			return "ASR_OK";
		case SOUNDAI_TIMEOUT:
			return "ASR_VAD_BEGIN_TIMEOUT";
		case SOUNDAI_BAD_REQUEST:
			return "ASR_BAD_REQUEST";
		case SOUNDAI_SERVER_ERROR:
			return "ASR_SERVER_ERROR";
		case SOUNDAI_ASR_FINAL_RESULT:
			return "ASR_FINAL_RESULT";
		case SOUNDAI_ASR_TEMP_RESULT:
			return "ASR_TEMP_RESULT";
	}
	return "unknown codeInfo";
}


/**
 * Write an @c netEventType value to an @c ostream.
 *
 * @param stream The stream to write the value to.
 * @param eventType The @c netEventType value to write to the @c ostream as a string.
 * @return The @c ostream that was passed in and written to.
 */
inline std::ostream& operator<<(std::ostream& stream, const netEventType& eventType) {
	return stream << eventTypeToString(eventType);
}

/**
 * Write an @c netEventType value to an @c ostream.
 *
 * @param stream The stream to write the value to.
 * @param eventType The @c netEventType value to write to the @c ostream as a string.
 * @return The @c ostream that was passed in and written to.
 */
inline std::ostream& operator<<(std::ostream& stream, const codeInfo& rc) {
	return stream << codeInfoToString(rc);
}

}	//engine
}	//soundai
} // namespace aisdk
#endif //__NET_EVENT_TYPES_H_

