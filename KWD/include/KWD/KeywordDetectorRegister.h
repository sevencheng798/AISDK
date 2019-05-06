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

#ifndef __KEYWORDDETECTOR_REGISTER_H_
#define __KEYWORDDETECTOR_REGISTER_H_

#include <mutex>
#include <unordered_set>

//#include <Utils/SharedBuffer/BufferLayout.h>
//#include <DMInterface/KeyWordObserverInterface.h>
#include <KWD/GenericKeywordDetector.h>

namespace aisdk {
namespace kwd {
	
// A class that creates a @c GenericKeywordDetector.
class KeywordDetectorRegister {
public:
	/**
	 * Creates a @c KeywordDetector.
	 *
     * @param stream The stream of audio data. This should be formatted:
	 * - 16bit Linear PCM
	 * - 16kHz sample rate
	 * - 1 channels
     * @param keyWordObservers The observers to notify of keyword detections.
     * @param pathToInputFolder The path to the inputs folder containing data files needed by this application.
     *
     * @return An @c KeywordDetector if creation success or @c nullptr if creation failed.
	 */
	static std::unique_ptr<GenericKeywordDetector> create(
		std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
		std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>> keyWordObserver);
};

}  // namespace kwd
}  // namespace aisdk

#endif  // __GENERIC_KEYWORDDETECTOR_H_