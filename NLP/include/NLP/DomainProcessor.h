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

#ifndef __DOMAIN_PROCESSOR_H_
#define __DOMAIN_PROCESSOR_H_

#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <thread>

#include <Utils/Channel/ChannelObserverInterface.h>
#include <DMInterface/DomainHandlerInterface.h>
#include <DMInterface/DomainHandlerResultInterface.h>

#include "NLPDomain.h"
#include "DomainRouter.h"

namespace aisdk {
namespace nlp {

/**
 *  Class for processor and handling a stream of @c NLPDomain instances.
 */
class DomainProcessor {
	
public:
	/**
     * Constructor.
     */
    DomainProcessor(DomainRouter *domainRouter);

	/**
	 * Destructor.
	 */
	~DomainProcessor();

	bool onDomain(std::shared_ptr<NLPDomain> domain);

	void shutdown();
	
private:

	/**
	 * Handle used to identify @c DomainProcessor instances referenced by @c DomainHandlerResultInterface.
	 * 
	 */
	using ProcessorHandle = unsigned int;

	class DomainHandlerResult : public dmInterface::DomainHandlerResultInterface {
	public:
		// Constructor.
		DomainHandlerResult(
			ProcessorHandle processorHandler,
			std::shared_ptr<NLPDomain> domain);

		// @name DomainHandlerResulterInterface method:
		// @{
			void setCompleted() override;

			void setFailed(const std::string &describle) override;
		// @}
	private:
		// Handler of @DomainProcessor to forward the notifications to.
		ProcessorHandle m_processorHandle;

		// The @NLPDomain whose handing the result will be specified by the instances.
		std::shared_ptr<NLPDomain> m_domainDirective;
	};

    /**
     * Receive notification that the handling of an @c NLPDomain has completed.
     *
     * @param domain The @c NLPDomain whose handling has completed.
     */
    void onHandlingCompleted(std::shared_ptr<NLPDomain> domain);

    /**
     * Receive notification that the handling of an @c NLPDomain has failed.
     *
     * @param domain The @c NLPDomain whose handling has failed.
     * @param description A description (suitable for logging diagnostics) that indicates the nature of the failure.
     */
    void onHandlingFailed(std::shared_ptr<NLPDomain> domain, const std::string& description);

    /**
     * Remove an be matched @c NLPDomain from processing.
     * @note This method must only be called by threads that have acquired @c m_mutex.
     *
     * @param domain The @c NLPDomain to remove from processing.
     */
    void removeDirectiveLocked(std::shared_ptr<NLPDomain> domain);
	
	/// Thread processor handler method.
	void processorLoop();

    /**
     * Process (cancel) all the items in @c m_cancelingQueue.
     *
     * @return Whether the @c NLPDomain in @c m_cancelingQueue were processed.
     */
	bool processorCancelingQueueDomainLocked(std::unique_lock<std::mutex> &lock);

    /**
     * Process (handle) the next @c NLPDomain in @c m_handlingQueue.
	 *
     * @return  Whether an @c NLPDomain from m_handlingQueue was processed.
     */
    bool handleDirectiveLocked(std::unique_lock<std::mutex>& lock);

	/// Handle value identifying this instance.
	ProcessorHandle m_handle;

	/// Object used to route directives to their assigned handler.
    DomainRouter* m_domainRouter;
		
    /// A mutex used to serialize @c DomainProcessor operations with operations that occur in the creating context.
    std::mutex m_mutex;

	/// Flags whether or not processor task is shutdown.
	bool m_isShuttingDown;

    /// Queue of @c NLPDomain waiting to be canceled.
    std::deque<std::shared_ptr<nlp::NLPDomain>> m_cancelingQueue;

    /// The directive (if any) for which a preHandleDirective() call is in progress.
    std::shared_ptr<nlp::NLPDomain> m_directiveBeingPreHandled;

	/// Queue of NLPDomain wait to be handled.
	std::deque<std::shared_ptr<nlp::NLPDomain>> m_handlingQueue;

    /// Whether @c handleDirective() has been called for the domain directive at the @c front() of @c m_handlingQueue.
    bool m_isHandlingDirective;

	/// condition variable used to wake when waiting.
	std::condition_variable m_wakeProcessingTask;

	/// Thread to receive processor handler.
	std::thread m_processingThread;

    /// Mutex serializing the body of @ onDomain() to make the method thread-safe.
    std::mutex m_onDomainMutex;

	/// Mutex to serialize access to @c m_handleMap;
    static std::mutex m_handleMapMutex;
		
    /**
     * Map from @c ProcessorHandle value to @c DomainProcessor instance to allow for gracefully dropping a
     * completion (or failure) notification forwarded to the @c DomainProcessor during or after its destruction.
     * Note: this is a gloab variable.
     */
    static std::unordered_map<ProcessorHandle, DomainProcessor*> m_handleMap;

    /// Next available @c ProcessorHandle value.
    static ProcessorHandle m_nextProcessorHandle;
};

}  // namespace nlp
}  // namespace aisdk

#endif  // __DOMAIN_PROCESSOR_H_
