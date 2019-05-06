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

#ifndef __APP_KEYWORD_OBSERVER_H_
#define __APP_KEYWORD_OBSERVER_H_

#include <memory>
#include <vector>
#include <DMInterface/KeyWordObserverInterface.h>
#include <Utils/SharedBuffer/SharedBuffer.h>

#include "Application/AIClient.h"

namespace aisdk {
namespace application {

/**
 * Observes callbacks from keyword detections and notifies the AIClient that a wake word has occurred or comein.
 */
class KeywordObserver : public dmInterface::KeyWordObserverInterface {
public:
    /**
     * Constructor.
     *
     * @param client The default SDK client.
     */
	KeywordObserver(std::shared_ptr<AIClient> client);

	void onKeyWordDetected(
	std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
	std::string keyword,
	utils::sharedbuffer::SharedBuffer::Index beginIndex = UNSPECIFIED_INDEX,
	utils::sharedbuffer::SharedBuffer::Index endIndex = UNSPECIFIED_INDEX) override;

private:
	/// The default SDK client.
	std::shared_ptr<AIClient> m_client;
};

}  // namespace application
}  // namespace aisdk

#endif  // __APP_KEYWORD_OBSERVER_H_

