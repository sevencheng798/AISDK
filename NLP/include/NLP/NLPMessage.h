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

#ifndef __NLP_MESSAGE_H_
#define __NLP_MESSAGE_H_

#include <memory>
#include <string>

namespace aisdk {
namespace nlp {

/**
 * This is a base class which allows us to represent a message sent or received from nlp/saisdk.
 * This class encapsulates the common data elements for all such messages.
 */
class NLPMessage {
public:
    /**
     * Constructor.
     *
     * @param code The code associated with NLP message.
     * @param message The message associated with NLP message.
     * @param query The query associated with NLP message.
     * @param domain The domain associated with NLP message.
     * @param data The data associated with an NLP message. This is expected to be in the JSON format.
     * @param messageId A unique ID used to identify a specific message. if NLP not support it,
     * 		The device must randomly generate a UUID as the messageId
     */
    NLPMessage(
    	const int code,
    	const std::string &message,
		const std::string &query,
		const std::string &domain,
		const std::string &data,
		const std::string &messageId):
		m_code{code},
		m_message{message},
		m_query{query},
		m_domain{domain},
	    m_data{data},
	    m_messageId{messageId} {
	    
	}

    /**
     * Destructor.
     */
    virtual ~NLPMessage() = default;

    /**
     * Returns The code of the message.
     *
     * @return The code.
     */
    int getCode() const;

	/**
     * Returns The message of the message.
     *
     * @return The message.
     */
	std::string getMessage() const;
	
    /**
     * Returns The domain type name of the message, which describes the intent.
     *
     * @return The domain name.
     */
    std::string getDomain() const;

    /**
     * Returns the query of the message.
     *
     * @return The query.
     */
    std::string getQuery() const;
    /**
     * Returns The message ID of the message.
     *
     * @return The message ID, a unique ID used to identify a specific message.
     */
    std::string getMessageId() const;

    /**
     * Returns the data (msg) of the message.
     *
     * @return The data.
     */
    std::string getData() const;

    /**
     * Return a string representation of this @c Message's header.
     *
     * @return A string representation of this @c Message's header.
     */
    //std::string getHeaderAsString() const;

private:
	/// The code of a NLP message. 
	const int m_code;

	/// The msg of a NLP message.
	const std::string m_message;

	/// The query of a NLP message.
	const std::string m_query;

	/// The query of a NLP message.
	const std::string m_domain;
	
    /// The data(msg) of a NLP message.
    const std::string m_data;

	/// The message ID, a unique ID used to identify a specific message
	const std::string m_messageId;
};

}  // namespace nlp
}  // namespace aisdk

#endif  // __NLP_MESSAGE_H_