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

#ifndef __DOMAIN_HANDLER_INTERFACE_H_
#define __DOMAIN_HANDLER_INTERFACE_H_

#include <memory>
#include <string>

#include "NLP/NLPDomain.h"
#include "DomainHandlerResultInterface.h"

namespace aisdk {
namespace dmInterface {

/**
 * This class interface for handling @c NLPDomain directive. receive messages from NLP/sai_sdk.
 */
class DomainHandlerInterface {
public:
	
    /**
     * Destructor.
     */
    virtual ~DomainHandlerInterface() = default;

	/**
     * Notification that a domain directive has arrived.  This notification gives the DomainHandler a chance to
     * prepare for handling the domain directive. 
     *
     * @param domain The domain to pre-handle.
     * @param result An object to receive the result of the operation.
     */
    virtual void preHandleDomain(
    std::shared_ptr<nlp::NLPDomain> domain,
    std::unique_ptr<DomainHandlerResultInterface> result) = 0;
	
    /**
     * Handle the action specified by the domain directive identified by @c messageId.
     *
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c NLPDomains.
     *
     * @param messageId The message ID of a domain directive previously passed to @c preHandleDomain().
     * @return @c false when @c messageId is not recognized, else @c true.  Any errors related to handling of a valid
     * messageId should be reported using @c DomainHandlerResultInterface::setFailed().
     */
	virtual bool handleDomain(const std::string& messageId) = 0;

    /**
     * Cancel an ongoing @c preHandleDomain() or @c handleDomain() operation for the specified @c NLPDomain.
     * Once this has been called the @c DomainHandler should not expect to receive further calls regarding this
     * directive.
     *
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c NLPDomains.
     *
     * @param messageId The message ID of a domain directive previously passed to preHandleDomain().
     */
	virtual void cancelDomain(const std::string& messageId) = 0;

    /**
     * Notification that this handler has been de-registered and will not receive any more calls.
     */
	virtual void onDeregistered() = 0;

	/**
	 * Get the name of the @c DomainHandlerInterface for the specified Handler. 
	 *
	 *@return @c The name of handler.
	 */
	virtual std::string getHandlerName() const = 0;
};

}  // namespace dmInterface
}  // namespace aisdk

#endif  // __DOMAIN_HANDLER_INTERFACE_H_