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

#ifndef __DOMAIN_SEQUENCER_INTERFACE_H_
#define __DOMAIN_SEQUENCER_INTERFACE_H_

#include <memory>
#include <string>

#include "Utils/SafeShutdown.h"
#include "NLP/NLPDomain.h"
#include "DMInterface/DomainHandlerInterface.h"

namespace aisdk {
namespace dmInterface {

/**
 * This class allows a client to receive messages from NLP/sai_sdk.
 */
class DomainSequencerInterface : public utils::SafeShutdown {
public:

	/// Constructor
	DomainSequencerInterface(const std::string& name);
	
    /**
     * Destructor.
     */
    virtual ~DomainSequencerInterface() = default;

    /**
     * Add the specified handler as a handler for its specified domain type.
     *
     * @param handler The handler to add.
     * @return Whether the handler was added.
     */
	virtual bool addDomainHandler(std::shared_ptr<DomainHandlerInterface> handler) = 0;

    /**
     * Remove the specified handler as a handler for its specified domain type.
     *
     * @param handler The handler to add.
     * @return Whether the handler was added.
     */
	virtual bool removeDomainHandler(std::shared_ptr<DomainHandlerInterface> handler) = 0;

    /**
     * Sequence the handling of an @c NLPDomain.  The actual handling is done by whichever @c DomainHandler
     * is associated with the @c NLPDomain's pair.
     *
     * @param domain The @c NLPDomain to handle.
     * @return Whether or not the directive was accepted.
     */
	virtual bool onDomain(std::shared_ptr<nlp::NLPDomain> domain) = 0;
};

inline DomainSequencerInterface::DomainSequencerInterface(const std::string& name)
	: utils::SafeShutdown{name} {
	
}

}  // namespace dmInterface
}  // namespace aisdk

#endif  // __DOMAIN_SEQUENCER_INTERFACE_H_