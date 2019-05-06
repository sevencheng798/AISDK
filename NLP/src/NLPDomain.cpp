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
// jsoncpp ver-1.8.3 
#include <json/json.h>
#include <Utils/Logging/Logger.h>

#include "NLP/NLPDomain.h"

/// String to identify log entries originating from this file.
static const std::string TAG("NLPDomain");
/// Define output
#define LX(event) aisdk::utils::logging::LogEntry(TAG, event)

namespace aisdk {
namespace nlp {

std::pair<std::unique_ptr<NLPDomain>, NLPDomain::ParseStatus> NLPDomain::create(
	const std::string &unparsedDomain,
	const std::string &messageId,
	std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker){
    std::pair<std::unique_ptr<NLPDomain>, ParseStatus> result;
	/// Default set the state as SUCCESS
    result.second = ParseStatus::SUCCESS;
	
	if(unparsedDomain.empty()){
		AISDK_WARN(LX("createFailed").d("reason", "nlpDomainNull"));
		result.second = ParseStatus::ERROR_INVALID_JSON;
		return result;
	}

	if(messageId.empty()){
		AISDK_WARN(LX("createFailed").d("reason", "messageIDNull"));
		result.second = ParseStatus::ERROR_INVALID_JSON;
		return result;
	}

	if(!attachmentDocker) {
		AISDK_ERROR(LX("createFailed").d("reason", "nullAttachmentManager"));
		return result;
	}
	
	Json::CharReaderBuilder readerBuilder;
	JSONCPP_STRING errs;
	Json::Value root;
	std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());	
	// Start parsing.
	if (!reader->parse(unparsedDomain.c_str(), unparsedDomain.c_str()+unparsedDomain.length(), &root, &errs)) {
		AISDK_ERROR(LX("createFailed").d("reason", "parseUnparsedDomainError").d("unparsedDomain", unparsedDomain));
		return result;
	}

	auto code = root["code"].asInt();
	auto message = root["message"].asString();
	auto query = root["query"].asString();
	auto domain = root["domain"].asString();

	Json::StreamWriterBuilder writerBuilder;
	std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
	std::ostringstream os;
	//std::string defaultParams;
	writer->write(root["data"], &os);
	auto data = std::string(os.str());
	
	AISDK_INFO(LX("Created")
			.d("code", code)
			.d("message", message)
			.d("query", query)
			.d("domain", domain)
			.d("data", data));

	if(domain == "chat" || \
	   domain == "baike" || \
	   domain == "weather") {
		domain.clear();
		domain = std::string("SpeechSynthesizer");
	}

	result.first = std::unique_ptr<NLPDomain>(
		new NLPDomain(attachmentDocker, unparsedDomain,
		code, message, query, domain, data, messageId));

	return result;
}

NLPDomain::NLPDomain(
	std::shared_ptr<utils::attachment::AttachmentManagerInterface> attachmentDocker,
	const std::string &unparsedDomain,
	const int code,
	const std::string &message,
	const std::string &query,
	const std::string &domain,
	const std::string &data,
	const std::string &messageId) : 
	NLPMessage(code, message, query, domain, data, messageId), 
	m_attachmentDocker{attachmentDocker},
	m_unparsedDomain{unparsedDomain}, 
	m_messageId{messageId} {

}

std::string NLPDomain::getUnparsedDomain() const {
	return m_unparsedDomain;
}

std::unique_ptr<utils::attachment::AttachmentReader>
NLPDomain::getAttachmentReader(
    const std::string& contentId,
    utils::sharedbuffer::ReaderPolicy readerPolicy) const {
	if(contentId != m_messageId) {
		AISDK_ERROR(LX("getAttachmentReaderFailed")
					.d("reason", "notMatchedContentId")
					.d("contentId", contentId)
					.d("messageId", m_messageId));
		return nullptr;
	}

	return m_attachmentDocker->createReader(contentId, readerPolicy);
}

}  // namespace nlp
}  // namespace aisdk