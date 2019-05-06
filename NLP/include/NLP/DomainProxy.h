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

#ifndef __DOMAIN_PROXY_H_
#define __DOMAIN_PROXY_H_

#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <mutex>

#include <Utils/Channel/ChannelObserverInterface.h>
#include <DMInterface/DomainHandlerInterface.h>

#include "NLPDomain.h"

namespace aisdk {
namespace nlp {

/**
 * This class DomainProxy implements methods which most capability agents will need @c DomainHandlerInterface.
 */
class DomainProxy
	: public dmInterface::DomainHandlerInterface
	, public utils::channel::ChannelObserverInterface {
	
public:

    /**
     * Virtual destructor
     */
    virtual ~DomainProxy() = default;

    /*
     * DomainHandlerInterface functions.
     *
     * The following four functions implement the @c DomainHandlerInterface. Only the message Id is passed to the
     * @c handleDomain and @c cancelDomain functions, so we need to maintain a mapping of the message Id to the
     * @c NLPDomain and @c DomainHandlerResultInterface, i.e., @c DomainAndResultInterface. The
     * @c DomainHandlerInterface functions call the functions of the same name with the
     * @c DomainAndResultInterface as the argument.
     */
	/// @{
	void preHandleDomain(
        std::shared_ptr<NLPDomain> domain,
        std::unique_ptr<dmInterface::DomainHandlerResultInterface> result) override final;

    bool handleDomain(const std::string& messageId) override final;

    void cancelDomain(const std::string& messageId) override final;

    void onDeregistered() override;
	/// @}

	/// @name ChannelObserverInterface.
	/// @{
    void onTrackChanged(utils::channel::FocusState newTrace) override;
	/// @}
	
protected:
    /**
     * Constructor for a Domain Agent.
     *
     * @param domainName The name of the Domain agent.
     */
    DomainProxy (const std::string& domainName);

	/**
     * DomainProxy maintains a map from messageId to instances of DirectiveInfo so that DomainProxy
     * can track the processing of an @c NLPDomain.
     */
    class DirectiveInfo {
    public:
        /**
         * Constructor.
         *
         * @param directiveIn The @c NLPDomain with which to populate this DirectiveInfo.
         * @param resultIn The @c DirectiveHandlerResultInterface instance with which to populate this DirectiveInfo.
         */
        DirectiveInfo(
            std::shared_ptr<NLPDomain> directiveIn,
            std::unique_ptr<dmInterface::DomainHandlerResultInterface> resultIn);

        /**
         * Destructor.
         */
        virtual ~DirectiveInfo() = default;

        /// The domain directive @c NLPDomain that is passed during preHandle.
        std::shared_ptr<NLPDomain> directive;

        /// @c DomainHandlerResultInterface.
        std::shared_ptr<dmInterface::DomainHandlerResultInterface> result;

        /// Flag to indicate whether the domain directive is cancelled.
        std::atomic<bool> isCancelled;
    };
	
    /**
     * Create a DirectiveInfo instance with which to track the handling of an @c NLPDomain.
     * @note This method is virtual to allow derived DomainProxy's to extend DirectiveInfo
     * with additional information.
     *
     * @param domain The @c NLPDomain to be processed.
     * @param result The object with which to communicate the outcome of processing the @c NLPDomain.
     * @return A DirectiveInfo instance with which to track the processing of @c domain directive.
     */
    virtual std::shared_ptr<DirectiveInfo> createDirectiveInfo(
        std::shared_ptr<NLPDomain> domain,
        std::unique_ptr<dmInterface::DomainHandlerResultInterface> result);

    /**
     * Notification that a directive has arrived.  This notification gives the DomainHandler a chance to
     * prepare for handling of an @c NLPDomain.
     *
     * @param info The @c DirectiveInfo instance for the @c NLPDomain to process.
     */
    virtual void preHandleDirective(std::shared_ptr<DirectiveInfo> info) = 0;

    /**
     * Handle the action specified by the @c NLPDomain in @c info.
	 *
     * @param info The @c DirectiveInfo instance for the @c NLPDomain to process.
     */
    virtual void handleDirective(std::shared_ptr<DirectiveInfo> info) = 0;

	/**
	 * Cancel an ongoing @c preHandleDomain() or @c handleDomain() operation for the specified @c NLPDomain.
	 * Once this has been called the @c DomainHandler should not expect to receive further calls regarding this
	 * directive.
     *
     * @param info The @c DirectiveInfo instance for the @c NLPDomain to process.
     */
    virtual void cancelDirective(std::shared_ptr<DirectiveInfo> info) = 0;

    /**
     * This function releases resources associated with the @c NLPDomain which is no longer in use by a
     * @c DomainProxy.
     *
     * @note This function should be called from @c handleDomain() and @c cancelDomain()
     * implementations after the outcome of @c handleDomain() or @c cancelDomain() has been reported.
     *
     * @param messageId The message Id of the @c NLPDomain.
     */
    void removeDirective(const std::string& messageId);

private:
	/// The name of the Domain agent.
    const std::string m_DomainSpace;

    /**
     * Find the DirectiveInfo instance (if any) for the specified messsageId.
     *
     * @param messageId The messageId value to find DirectiveInfo for.
     * @return The DirectiveInfo instance for @c messageId.
     */
    std::shared_ptr<DirectiveInfo> getDirectiveInfo(const std::string& messageId);

    /// Map of message Id to @c DomainDirectiveAndResultInterface.
    std::unordered_map<std::string, std::shared_ptr<DirectiveInfo>> m_directiveInfoMap;

    /// Mutex to protect message Id to @c DomainDirectiveAndResultInterface mapping.
    std::mutex m_mutex;		
};

}  // namespace nlp
}  // namespace aisdk

#endif  // __DOMAIN_PROXY_H_
