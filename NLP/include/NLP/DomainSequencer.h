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

#ifndef __DOMAIN_SEQUENCER_H_
#define __DOMAIN_SEQUENCER_H_

#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <mutex>

#include <Utils/Channel/ChannelObserverInterface.h>
#include <DMInterface/DomainHandlerInterface.h>
#include <DMInterface/DomainSequencerInterface.h>

#include "NLPDomain.h"
#include "DomainProcessor.h"
#include "DomainRouter.h"

namespace aisdk {
namespace nlp {

/**
 * Class for sequencing and handling a stream of @c NLPDomain instances.
 * Note: Should ensure that only one instance is referenced.
 */
class DomainSequencer
	: public dmInterface::DomainSequencerInterface {
	
public:
	/**
	 * Create a DomainSequencer
	 * @return Return a new DomainSequencer, or nullptr if the opreation failed.
	 */
	static std::unique_ptr<dmInterface::DomainSequencerInterface> create();
    
    /// DomainSequencerInterface functions.
	/// @{
	bool addDomainHandler(std::shared_ptr<dmInterface::DomainHandlerInterface> handler) override;

    bool removeDomainHandler(std::shared_ptr<dmInterface::DomainHandlerInterface> handler) override;

	bool onDomain(std::shared_ptr<nlp::NLPDomain> domain) override;
	/// @}
	
private:
    /**
     * Constructor.
     */
    DomainSequencer();

	void doShutdown() override;

	/// Thread sequencer handler method.
	void receivingLoop();

    /**
     * Process the next item in @c m_receivingQueue.
     * @note This method must only be called by threads @c receivingLoop() that have acquired @c m_mutex.
     *
     * @param lock A @c unique_lock on m_mutex, allowing this method to release the lock around callbacks
     * that need to be invoked.
     */	
	void receiveDomainLocked(std::unique_lock<std::mutex> &lock);

	/// Object used to routed domain directives to their assigned handler.
	DomainRouter m_domainRouter;

	/// Object used to drive handling of @c NLPDomain.
	std::shared_ptr<DomainProcessor> m_domainProcessor;
		
	/// Flags whether or not receive task is shutdown.
	bool m_isShuttingDown;
	
	/// Thread to receive sequencer handler.
	std::thread m_receivingThread;

	/// Queue of NLPDomain to wait be received.
	std::deque<std::shared_ptr<nlp::NLPDomain>> m_receivingQueue;

	/// condition variable used to wake when waiting.
	std::condition_variable m_wakeReceivingTask;
		
    /// Mutex to protect message Id to @c DomainDirectiveAndResultInterface mapping.
    std::mutex m_mutex;
};

}  // namespace nlp
}  // namespace aisdk

#endif  // __DOMAIN_SEQUENCER_H_
