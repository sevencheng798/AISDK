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
#include <vector>
#include <Utils/Logging/Logger.h>

#include "NLP/DomainRouter.h"

/// String to identify log entries originating from this file. 
static const std::string TAG("DomainRouter");

#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace nlp {

DomainRouter::DomainRouter() : utils::SafeShutdown{"DomainRouter"} {

}

bool DomainRouter::addDomainHandler( std::shared_ptr<dmInterface::DomainHandlerInterface> handler) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if(isShutdown()){
		AISDK_ERROR(LX("addDomainHandlerFailed").d("reason", "isShutdown"));
		return false;
	}
	
	if(!handler) {
		AISDK_ERROR(LX("addDomainHandlerFailed").d("reason", "emptyHandler"));
		return false;
	}

	auto name = handler->getHandlerName();
	auto it = m_configuration.find(name);
	if(it != m_configuration.end()) {
		AISDK_WARN(LX("addDomainHandlerFailed")
			.d("reason", "alreadySet")
			.d("domainName", name)
			.d("handler", handler.get()));
		
		return false;
	}

	m_configuration[name] = handler;
	AISDK_DEBUG5(LX("addDomainHandler")
		.d("action", "added")
		.d("domainName", name)
		.d("handler", handler.get()));

	return true;
}

bool DomainRouter::removeDomainHandlerLocked(std::shared_ptr<dmInterface::DomainHandlerInterface> handler) {
	if(!handler) {
		AISDK_ERROR(LX("removeDomainHandlerLockedFailed").d("reason", "emptyHandler"));
		return false;
	}

	auto name = handler->getHandlerName();
	auto it = m_configuration.find(name);
	if(it == m_configuration.end()) {
		AISDK_ERROR(LX("removeDomainHandlerLockedFailed").d("reason", "notFound").d("name", name));
		return false;
	}

	//
	m_configuration.erase(name);
	AISDK_DEBUG5(LX("removeDomainHandlerLocked")
		.d("action", "removed")
		.d("domainName", name)
		.d("handler", handler.get()));
	
	return true;
}

bool DomainRouter::removeDomainHandler( std::shared_ptr<dmInterface::DomainHandlerInterface> handler) {
	std::unique_lock<std::mutex> lock(m_mutex);
	
	if(!removeDomainHandlerLocked(handler)) {
		return false;
	}
	
	lock.unlock();
	AISDK_DEBUG5(LX("onDeregisteredCalled").d("handler", handler.get()));
	handler->onDeregistered();

	return true;
}

std::shared_ptr<dmInterface::DomainHandlerInterface> DomainRouter::getDomainHandlerLocked(
	std::shared_ptr<NLPDomain> domain) {
	auto name = domain->getDomain();
	auto it = m_configuration.find(name);
	if(it == m_configuration.end()) {
		AISDK_WARN(LX("getDomainHandlerLockedFailed").d("domainName", name).d("reason", "noHandlerRegistered"));
		return nullptr;
	}

	return it->second;
}

bool DomainRouter::preHandleDomain(
	std::shared_ptr<NLPDomain> domain,
	std::unique_ptr<dmInterface::DomainHandlerResultInterface> result) {
	std::unique_lock<std::mutex> lock(m_mutex);
	auto handler = getDomainHandlerLocked(domain);
	if(!handler) {
		return false;
	}

	handler->preHandleDomain(domain, std::move(result));

	return true;
}

bool DomainRouter::handleDomain(std::shared_ptr<NLPDomain> domain) {
	std::unique_lock<std::mutex> lock(m_mutex);
	auto handler = getDomainHandlerLocked(domain);
	if(!handler) 
		return false;

	auto result = handler->handleDomain(domain->getMessageId());
	if(!result) {
        AISDK_WARN(LX("messageIdNotRecognized")
               .d("handler", handler.get())
               .d("messageId", domain->getMessageId())
               .d("reason", "handleDomainReturnedFalse"));
	}

	return result;
}

bool DomainRouter::cancelDomain(std::shared_ptr<NLPDomain> domain) {
	std::unique_lock <std::mutex> lock(m_mutex);
	auto handler = getDomainHandlerLocked(domain);
	if(!handler)
		return false;

	AISDK_INFO(LX("cancelDomain").d("messageId", domain->getMessageId()).d("action", "calling"));
	handler->cancelDomain(domain->getMessageId());
	return true;
}

void DomainRouter::doShutdown() {
	std::vector<std::shared_ptr<dmInterface::DomainHandlerInterface>> releasedHandlers;
	std::unique_lock<std::mutex> lock(m_mutex);
	for(auto &it : m_configuration) {
		auto handler = it.second;
		if(removeDomainHandlerLocked(handler)) {
			releasedHandlers.push_back(handler);
		}
	}
	/// Clear map space to call destructor.
	m_configuration.clear();
	lock.unlock();

    for (auto releasedHandler : releasedHandlers) {
        AISDK_DEBUG5(LX("onDeregisteredCalled").d("handler", releasedHandler.get()));
        releasedHandler->onDeregistered();
    }
}

}  // namespace nlp
}  // namespace aisdk
