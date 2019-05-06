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

#include <Utils/LibcurlUtils/HTTPContentFetcherFactory.h>
#include <Utils/LibcurlUtils/LibCurlHttpContentFetcher.h>
#include <Utils/Threading/Memory.h>

namespace aisdk {
namespace utils {
namespace libcurlUtils {

std::unique_ptr<dmInterface::HTTPContentFetcherInterface> HTTPContentFetcherFactory::create(
    const std::string& url) {
    return utils::memory::make_unique<LibCurlHttpContentFetcher>(url);
}

}  // namespace libcurlUtils
}  // namespace utils
}  // namespace aisdk
