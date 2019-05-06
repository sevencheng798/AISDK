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

#ifndef __DOMAIN_ROUTER_H_
#define __DOMAIN_ROUTER_H_

#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <mutex>

#include <DMInterface/DomainHandlerInterface.h>
#include <Utils/SafeShutdown.h>

#include "NLPDomain.h"

namespace aisdk {
namespace nlp {

/**
 *  Class for sequencing and handling a stream of @c NLPDomain instances.
 */
class DomainRouter
	: public utils::SafeShutdown {
	
public:
	/**
	 * Create a DomainRouter.
	 */
	DomainRouter();

	/**
     * Add mappings from from handler's @c DomainName values gotten through the handler's getConfiguration() method.
     * If a mapping for any of the specified @c DomainName values already
     * exists the entire call is refused.
     *
     * @param handler The handler to add.
     * @return Whether the handler was added.
     */
	bool addDomainHandler(std::shared_ptr<dmInterface::DomainHandlerInterface> handler);

    /**
     * Remove the specified mappings from @c DomainName values.
     *
     * @param handler The handler to remove.
     * @return Whether the configuration was removed.
     */
	bool removeDomainHandler(std::shared_ptr<dmInterface::DomainHandlerInterface> handler);

    /**
     * Invoke @c preHandleDomain() on the handler registered for the given @c NLPDomain.
     *
     * @param domain The domain directive to be preHandled.
     * @param result A result object to receive notification about the completion (or failure) of handling
     * the @c NLPDomain.
     * @return Whether or not the handler was invoked.
     */
	bool preHandleDomain(
		std::shared_ptr<NLPDomain> domain,
		std::unique_ptr<dmInterface::DomainHandlerResultInterface> result);

    /**
     * Invoke @c handleDomain() on the handler registered for the given @c NLPDomain.
     *
     * @param domain The domain directive to be handled.
     * @return @c true if the the registered handler returned @c true.  @c false if there was no registered handler
     * or the registered handler returned @c false (indicating that the directive was not recognized.
     */
	bool handleDomain(std::shared_ptr<NLPDomain> domain);

    /**
     * Invoke cancelDomain() on the handler registered for the given @c NLPDomain.
     *
     * @param domain The domain directive to be handled.
     * @return Whether or not the handler was invoked.
     */
	bool cancelDomain(std::shared_ptr<NLPDomain> domain);
	
private:
	/// SafeShutdown method.
	void doShutdown() override;

    /**
     * Look up the @c Handler value for the specified @c NLPDomain.
     *
     * @param domain The domain directive to look up a value for.
     * @return The corresponding @c Handler value for the specified domain directive.
     */
	std::shared_ptr<dmInterface::DomainHandlerInterface> getDomainHandlerLocked(
	std::shared_ptr<NLPDomain> domain);

	/**
     * Remove the specified mappings from the name of the @c DomainHandlerInterface's values, gotten through the
     * handler's getHandlerName() method. If any of the specified mappings do not match an existing mapping, the
     * entire operation is refused.  This function should be called while holding m_mutex.
     *
     * @param handler The handler to remove.
     * @return @c true if @c handler was removed successfully, else @c false indicates an error occurred.
     */
    bool removeDomainHandlerLocked(std::shared_ptr<dmInterface::DomainHandlerInterface> handler);
		
    /// Mutex to protect message Id to @c DomainDirectiveAndResultInterface mapping.
    std::mutex m_mutex;

	/// Mapping from @c HandlerName to @c Handler.
    std::unordered_map<std::string, std::shared_ptr<dmInterface::DomainHandlerInterface>> m_configuration;
};

}  // namespace nlp
}  // namespace aisdk

#endif  // __DOMAIN_ROUTER_H_