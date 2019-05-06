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
#ifndef __MESSAGE_CONSUMER_H_
#define __MESSAGE_CONSUMER_H_
#include <memory>
#include <mutex>

#include <DMInterface/MessageObserverInterface.h>
#include <DMInterface/MessageConsumerInterface.h>

namespace aisdk {
namespace asr {

class MessageConsumer : public dmInterface::MessageConsumerInterface {
public:
	/// Constructor
	MessageConsumer(
		std::shared_ptr<dmInterface::MessageObserverInterface> messageObserver);

	///
	//setObserver(std::shared_ptr<dmInterface::MessageObserverInterface> observer);

	/// MessageConsumerInterface method.
	void consumeMessage(const std::string& contextId, const std::string& message) override;

private:

	/// The message listener, which will receive all messages sent from sai sdk.
	std::shared_ptr<dmInterface::MessageObserverInterface> m_observer;

	/// Mutex to serialize access to m_observer.
	std::mutex m_mutex;
};

}	//asr
} // namespace aisdk
#endif //__MESSAGE_CONSUMER_H_

