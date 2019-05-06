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
 #include "Application/KeywordObserver.h"

namespace aisdk {
namespace application {

KeywordObserver::KeywordObserver(std::shared_ptr<AIClient> client):m_client{client} {

}

void KeywordObserver::onKeyWordDetected(
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
	std::string keyword,
	utils::sharedbuffer::SharedBuffer::Index beginIndex,
	utils::sharedbuffer::SharedBuffer::Index endIndex) {

	if (m_client) {
	   m_client->notifyOfWakeWord(stream, beginIndex, endIndex);
   }

}


}  // namespace application
}  // namespace aisdk

