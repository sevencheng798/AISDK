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

#ifndef __MESSAGE_INTERPRETER_H_
#define __MESSAGE_INTERPRETER_H_

#include <memory>

#include <DMInterface/DomainSequencerInterface.h>
#include <DMInterface/MessageObserverInterface.h>

namespace aisdk {
namespace nlp {

/**
 * Class that converts message(The msg which from NLP/sai_sdk respond) to @c NLPDomain, 
 * and passes those domains to a @c DomainSequencerInterface.
 */
class MessageInterpreter : public dmInterface::MessageObserverInterface {
public:
    /**
     * Constructor.
     *
     * @param domainSequencerInterface The DomainSequencerInterface implementation, which will receive
     *        @c NLPDirectives.
     * @param attachmentDocker The @c AttachmentManager which created @c NLPDomain will use to acquire Attachments.
     */
    MessageInterpreter(
        std::shared_ptr<dmInterface::DomainSequencerInterface> domainSequencer,
        std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker);

    void receive(const std::string& contextId, const std::string& message) override;

private:
    /// Object to which we will send @c NLPDirectives.
    std::shared_ptr<dmInterface::DomainSequencerInterface> m_domainSequencer;

	/// The attachmentManager.
	std::shared_ptr<utils::attachment::AttachmentManagerInterface> m_attachmentDocker;
};

}  // namespace nlp
}  // namespace aisdk

#endif  // __MESSAGE_INTERPRETER_H_