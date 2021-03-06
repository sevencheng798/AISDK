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

#ifndef __MESSAGE_CONSUME_INTERFACE_H_
#define __MESSAGE_CONSUME_INTERFACE_H_

#include <memory>

namespace aisdk {
namespace dmInterface {

/**
 * An interface which allows a derived class to consume a Message from Sai sdk.
 */
class MessageConsumerInterface {
public:
    /**
     * Destructor.
     */
    virtual ~MessageConsumerInterface() = default;

    /**
     * Called when a message has been received from Sai sdk.
     *
     * @param contextId The context id for the current message.
     * @param message The semantics message in string representation.
     */
    virtual void consumeMessage(const std::string& contextId, const std::string& message) = 0;
};

}  // namespace dmInterface
}  // namespace aisdk

#endif  // __MESSAGE_CONSUME_INTERFACE_H_