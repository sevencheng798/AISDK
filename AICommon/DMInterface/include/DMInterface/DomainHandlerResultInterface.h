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

#ifndef __DOMAIN_HANDLER_RESULT_INTERFACE_H_
#define __DOMAIN_HANDLER_RESULT_INTERFACE_H_

#include <string>

namespace aisdk {
namespace dmInterface {

/**
 * This class allows a client to receive messages from NLP/sai_sdk.
 */
class DomainHandlerResultInterface {
public:

    /**
     * Virtual destructor to ensure proper cleanup by derived types.
     */
    virtual ~DomainHandlerResultInterface() = default;

    /**
     * Indicate that the handling of the @c NLPDomain successfully ran to successful completion.
     *
     * @note Once this has been called, no other methods of this object should be called.
     */
    virtual void setCompleted() = 0;

    /**
     * Indicate that handling an @c NLPDomain failed.
     *
     * @note Once this has been called, no other methods of this object should be called.
     *
     * @param description A description, suitable for logging, that indicates the nature of the failure.
     */
    virtual void setFailed(const std::string& description) = 0;
};

}  // namespace dmInterface
}  // namespace aisdk

#endif  // __DOMAIN_HANDLER_RESULT_INTERFACE_H_