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
#include <algorithm>

#include <Utils/Logging/Logger.h>
#include <Utils/Threading/Memory.h>

#include "NLP/DomainProcessor.h"

/// String to identify log entries originating from this file. 
static const std::string TAG("DomainProcessor");

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace nlp {

std::mutex DomainProcessor::m_handleMapMutex;
DomainProcessor::ProcessorHandle DomainProcessor::m_nextProcessorHandle = 0;
std::unordered_map<DomainProcessor::ProcessorHandle, DomainProcessor*> DomainProcessor::m_handleMap;

DomainProcessor::DomainProcessor(DomainRouter *domainRouter):
	m_domainRouter{domainRouter},
	m_isShuttingDown{false},
	m_isHandlingDirective{false} {
	std::lock_guard<std::mutex> lock(m_handleMapMutex);
	m_handle = ++m_nextProcessorHandle;
	m_handleMap[m_handle] = this;

	m_processingThread = std::thread(&DomainProcessor::processorLoop, this);
}

DomainProcessor::~DomainProcessor() {
	shutdown();
}

bool DomainProcessor::onDomain(std::shared_ptr<NLPDomain> domain) {
	if (!domain) {
        AISDK_ERROR(LX("onDomainFailed").d("action", "ignored").d("reason", "nullptrDomainDirective"));
        return false;
    }

	std::lock_guard<std::mutex> lockDomain(m_onDomainMutex);
	std::unique_lock<std::mutex> lock(m_mutex);
	if(m_isShuttingDown) {
		AISDK_WARN(LX("onDomainFailed").d("domain", domain->getMessageId()).d("action", "ignored").d("reason", "shuttingDown"));
		return false;		
	}

	m_directiveBeingPreHandled = domain;
	lock.unlock();
    auto handled = m_domainRouter->preHandleDomain(
        domain, utils::memory::make_unique<DomainHandlerResult>(m_handle, domain));
    lock.lock();

	if(m_directiveBeingPreHandled) {
		m_directiveBeingPreHandled.reset();
		if(handled) {
			m_handlingQueue.push_back(domain);
			m_wakeProcessingTask.notify_one();
		}
	}

	return handled;
}

// Be called by @c domainSequencer
void DomainProcessor::shutdown() {
	{
		std::lock_guard<std::mutex> lock(m_handleMapMutex);
		m_handleMap.erase(m_handle);
	}
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_isShuttingDown = true;
		m_wakeProcessingTask.notify_one();
	}

	if(m_processingThread.joinable())
		m_processingThread.join();

	// Should clear mpa space - temp mark.
	m_handlingQueue.clear();
}

DomainProcessor::DomainHandlerResult::DomainHandlerResult(
	DomainProcessor::ProcessorHandle processorHandler,
	std::shared_ptr<NLPDomain> domain):
	m_processorHandle{processorHandler},
	m_domainDirective{domain} {

}

void DomainProcessor::DomainHandlerResult::setCompleted() {
	std::lock_guard<std::mutex> lock(m_handleMapMutex);
	auto it = m_handleMap.find(m_processorHandle);
	if(it == m_handleMap.end()) {
		AISDK_DEBUG0(LX("setCompletedIgnored").d("reason", "domainSequencerAlreadyShutdown"));
		return;
	}

	it->second->onHandlingCompleted(m_domainDirective);
}

void DomainProcessor::DomainHandlerResult::setFailed(const std::string &describle) {
	std::lock_guard<std::mutex> lock(m_handleMapMutex);
	auto it = m_handleMap.find(m_processorHandle);
	if(it == m_handleMap.end()) {
		AISDK_DEBUG0(LX("setFailedIgnored").d("reason", "domainSequencerAlreadyShutdown"));
		return;
	}

	it->second->onHandlingFailed(m_domainDirective, describle);
}

void DomainProcessor::onHandlingCompleted(std::shared_ptr<NLPDomain> domain) {
	std::lock_guard<std::mutex> lock(m_mutex);
    AISDK_DEBUG(LX("onHandlingCompeted")
                    .d("messageId", domain->getMessageId())
                    .d("domainBeingPreHandled",
                       m_directiveBeingPreHandled ? m_directiveBeingPreHandled->getMessageId() : "(nullptr)"));

    removeDirectiveLocked(domain);
}

void DomainProcessor::onHandlingFailed(std::shared_ptr<NLPDomain> domain, const std::string& description) {
    std::unique_lock<std::mutex> lock(m_mutex);
    AISDK_DEBUG(LX("onHandlingFailed")
                    .d("messageId", domain->getMessageId())
                    .d("domainBeingPreHandled",
                       m_directiveBeingPreHandled ? m_directiveBeingPreHandled->getMessageId() : "(nullptr)")
                    .d("description", description));

    removeDirectiveLocked(domain);
    //scrubDialogRequestIdLocked(domain->getDialogRequestId()); tmp Sven
}

void DomainProcessor::removeDirectiveLocked(std::shared_ptr<NLPDomain> domain) {
    auto matches = [domain](std::shared_ptr<NLPDomain> item) { return item == domain; };

    m_cancelingQueue.erase(
        std::remove_if(m_cancelingQueue.begin(), m_cancelingQueue.end(), matches), m_cancelingQueue.end());

	// Make sure @c m_directiveBeingPreHandled is clear.
    if (matches(m_directiveBeingPreHandled)) {
        m_directiveBeingPreHandled.reset();
    }

	// Check and remove matching @c NLPDomain from @c m_handlingQueue. 
    if (m_isHandlingDirective && !m_handlingQueue.empty() && matches(m_handlingQueue.front())) {
        m_isHandlingDirective = false;
        m_handlingQueue.pop_front();
    }

    m_handlingQueue.erase(
        std::remove_if(m_handlingQueue.begin(), m_handlingQueue.end(), matches), m_handlingQueue.end());

	// Next process handle
    if (!m_cancelingQueue.empty() || !m_handlingQueue.empty()) {
        m_wakeProcessingTask.notify_one();
    }

}

void DomainProcessor::processorLoop() {
    auto wake = [this]() {
        return !m_cancelingQueue.empty() || (!m_handlingQueue.empty() && !m_isHandlingDirective) || m_isShuttingDown;
    };

    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_wakeProcessingTask.wait(lock, wake);
        if (!processorCancelingQueueDomainLocked(lock) && !handleDirectiveLocked(lock) && m_isShuttingDown) {
            break;
        }
    }
}

bool DomainProcessor::processorCancelingQueueDomainLocked(std::unique_lock<std::mutex> &lock) {
    if (m_cancelingQueue.empty()) {
        return false;
    }
    std::deque<std::shared_ptr<NLPDomain>> temp(std::move(m_cancelingQueue));
    lock.unlock();
    for (auto domain : temp) {
        m_domainRouter->cancelDomain(domain);
    }
    lock.lock();
    return true;
}

bool DomainProcessor::handleDirectiveLocked(std::unique_lock<std::mutex>& lock) {
    if (m_handlingQueue.empty()) {
        return false;
    }
	
    if (m_isHandlingDirective) {
        return true;
    }
	
    auto domain = m_handlingQueue.front();
    m_isHandlingDirective = true;
    lock.unlock();
    auto handled = m_domainRouter->handleDomain(domain);
    lock.lock();
    if (!handled) {
  		AISDK_ERROR(LX("handlingDirectiveLockedFailed")
                    .d("expected", domain->getMessageId())
                    .d("front", m_handlingQueue.empty() ? "(empty)" : m_handlingQueue.front()->getMessageId())
                    .d("reason", "handlingQueueFrontChangedWithoutBeingHandled"));
    }

    return true;   
}

}  // namespace nlp
}  // namespace aisdk
