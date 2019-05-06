/*
 * Copyright 2019 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef _UTILS_HTTPCONTENT_H_
#define _UTILS_HTTPCONTENT_H_

#include <chrono>
#include <future>
#include <memory>
#include <string>

namespace aisdk {
namespace utils {

/**
 * This struct encapsulates content received from HTTP request, specifically the status code, the content-type, and the
 * actual content of the response.
 */
struct HTTPContent {
    /**
     * This function blocks until @c statusCode is set and checks whether it is equal to 200, indicating an HTTP sucess
     * code.
     *
     * @return @c true if `statuscode == 200`, else @c false.
     */
    operator bool() const;

    /**
     * This function checks if the @c statusCode is ready to be read.
     *
     * @param timeout The timeout to wait for to see if @c statusCode is ready.
     * @return @c true if @c statuscode is ready, else @c false.
     */
    bool isReady(const std::chrono::milliseconds timeout) const;

    /// A @c long representing the HTTP status code.
    mutable std::future<long> statusCode;

    /// A @c string representing the content-type of the HTTP content.
    std::future<std::string> contentType;

    /// An @c Attachment from which to read the HTTP content from or @c nullptr if no data was fetched.
    //std::shared_ptr<avsCommon::avs::attachment::InProcessAttachment> dataStream;
};

inline HTTPContent::operator bool() const {
    return statusCode.get() == 200;
}

inline bool HTTPContent::isReady(const std::chrono::milliseconds timeout) const {
    auto status = statusCode.wait_for(timeout);
    return std::future_status::ready == status;
}

}  // namespace utils
}  // namespace aisdk

#endif  // _UTILS_HTTPCONTENT_H_