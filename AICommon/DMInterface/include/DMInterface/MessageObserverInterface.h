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

#ifndef __MESSAGE_OBSERVER_INTERFACE_H_
#define __MESSAGE_OBSERVER_INTERFACE_H_

#include <string>

namespace aisdk {
namespace dmInterface {

/**
 * This class allows a client to receive messages from NLP/sai_sdk.
 */
class MessageObserverInterface {
public:
    /**
     * Destructor.
     */
    virtual ~MessageObserverInterface() = default;

    /**
     * A function that a client must implement to receive Messages from NLP/sai_sdk.
     *
     * @param contextId The context for the message, which in this case reflects the logical respond stream the
     * message arrived on.
     * @param message The NLP/sai_sdk message that has been received.
     */
    virtual void receive(const std::string& contextId, const std::string& message) = 0;
};

}  // namespace dmInterface
}  // namespace aisdk

#endif  // __MESSAGE_OBSERVER_INTERFACE_H_