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

#include <Utils/Logging/Logger.h>
#include "NLP/MessageInterpreter.h"

/// String to identify log entries originating from this file.
static const std::string TAG{"MessageInterpreter"};
/// define output
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace nlp {

MessageInterpreter::MessageInterpreter(
	std::shared_ptr<dmInterface::DomainSequencerInterface> domainSequencer,
	std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker):
	m_domainSequencer{domainSequencer},
	m_attachmentDocker{attachmentDocker} {

}

void MessageInterpreter::receive(const std::string& contextId, const std::string& message) {
    auto createResult = NLPDomain::create(message, contextId, m_attachmentDocker);
    std::shared_ptr<NLPDomain> nlpDomain{std::move(createResult.first)};
    if (!nlpDomain) {
		std::string descriptMsg = nlpDomainParseStatusToString(createResult.second);		
		AISDK_WARN(LX("receiveFailed").d("Unable to parse Directive - JSON error", descriptMsg));
		//std::cout << "Unable to parse Directive - JSON error: " << std::endl;    
		// we need to operator<<
//		std::cout << "Unable to parse Directive - JSON error: " << createResult.second << std::endl;
        return;
    }

	/// submit a new domain event to domain directive process sequencer.
    m_domainSequencer->onDomain(nlpDomain);
}

}  // namespace nlp
}  // namespace aisdk