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
#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <mutex>

#include <Utils/Channel/ChannelObserverInterface.h>
#include <DMInterface/DomainHandlerInterface.h>
#include <Utils/Logging/Logger.h>

#include "NLP/DomainSequencer.h"

/// String to identify log entries originating from this file. 
static const std::string TAG("DomainSequencer");

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace nlp {

std::unique_ptr<dmInterface::DomainSequencerInterface> DomainSequencer::create() {
	return std::unique_ptr<dmInterface::DomainSequencerInterface>(new DomainSequencer());
}

bool DomainSequencer::addDomainHandler(std::shared_ptr<dmInterface::DomainHandlerInterface> handler) {
	return m_domainRouter.addDomainHandler(handler);
}

bool DomainSequencer::removeDomainHandler(std::shared_ptr<dmInterface::DomainHandlerInterface> handler) {
	return m_domainRouter.removeDomainHandler(handler);
}

DomainSequencer::DomainSequencer()
	: dmInterface::DomainSequencerInterface{"DomainSequencer"},
	m_isShuttingDown{false},
	m_mutex{} {

	m_domainProcessor = std::make_shared<DomainProcessor>(&m_domainRouter);
	m_receivingThread = std::thread(&DomainSequencer::receivingLoop, this);
}

bool DomainSequencer::onDomain(std::shared_ptr<nlp::NLPDomain> domain) {
	if(!domain){
		AISDK_ERROR(LX("onDomainFailed").d("action", "ignored").d("reason", "nullNLPDomaindirective"));
		return false;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_isShuttingDown) {
		AISDK_WARN(LX("onDomainFailed").d("domain", domain->getUnparsedDomain()).d("action", "ignored").d("reason", "shuttingDown"));
		return false;
	}
	
	m_receivingQueue.push_back(domain);
	m_wakeReceivingTask.notify_one();

	return true;
}

void DomainSequencer::doShutdown() {
	AISDK_DEBUG0(LX("doShutdown"));
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_isShuttingDown = true;
		m_wakeReceivingTask.notify_one();
	}

	if(m_receivingThread.joinable())
		m_receivingThread.join();

	m_domainProcessor->shutdown();
	m_domainRouter.shutdown();
	m_receivingQueue.clear();
}

void DomainSequencer::receivingLoop() {
#if 0
	auto wake = [this](){
		return !m_receivingQueue.empty();
	};
#endif
	std::unique_lock<std::mutex> lock(m_mutex);
	while(true){
		m_wakeReceivingTask.wait(lock, [this](){ return !m_receivingQueue.empty() || m_isShuttingDown; });
		if(m_isShuttingDown)
			break;
		
		receiveDomainLocked(lock);
	}
}

void DomainSequencer::receiveDomainLocked(std::unique_lock<std::mutex> &lock) {
	if(m_receivingQueue.empty()){
		return;
	}

	auto nlpDomain = m_receivingQueue.front();
	m_receivingQueue.pop_front();
	lock.unlock();

	auto handler = m_domainProcessor->onDomain(nlpDomain);
	if(!handler) {
		AISDK_ERROR(LX("receiveDomainLockedFailed").d("messageId", nlpDomain->getMessageId()));		
	}

	lock.lock();
}

}  // namespace nlp
}  // namespace aisdk
