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

#include "NLP/NLPMessage.h"

namespace aisdk {
namespace nlp {

int NLPMessage::getCode() const {
    return m_code;
}

std::string NLPMessage::getDomain() const {
    return m_domain;
}

std::string NLPMessage::getMessage() const {
    return m_message;
}

std::string NLPMessage::getQuery() const {
    return m_query;
}

std::string NLPMessage::getMessageId() const {
    return m_messageId;
}

std::string NLPMessage::getData() const {
    return m_data;
}

}  // namespace nlp
}  // namespace aisdk
