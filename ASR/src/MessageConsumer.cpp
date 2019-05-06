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

#include "ASR/MessageConsumer.h"

namespace aisdk {
namespace asr {


MessageConsumer::MessageConsumer(
	std::shared_ptr<dmInterface::MessageObserverInterface> messageObserver):m_observer{messageObserver} {

}

void MessageConsumer::consumeMessage(const std::string& contextId, const std::string& message) {
	 std::lock_guard<std::mutex> lock(m_mutex);
	 m_observer->receive(contextId, message);
}


}	//asr
} // namespace aisdk
