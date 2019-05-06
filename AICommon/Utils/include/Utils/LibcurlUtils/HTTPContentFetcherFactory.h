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

#ifndef _LIBCURLUTILS_HTTPCONTENTFETCHERFACTORY_H_
#define _LIBCURLUTILS_HTTPCONTENTFETCHERFACTORY_H_

#include <memory>
#include <string>

#include <DMInterface/HTTPContentFetcherInterface.h>
#include <DMInterface/HTTPContentFetcherInterfaceFactoryInterface.h>

namespace aisdk {
namespace utils {
namespace libcurlUtils {

/**
 * A class that produces @c HTTPContentFetchers.
 */
class HTTPContentFetcherFactory : public dmInterface::HTTPContentFetcherInterfaceFactoryInterface {
public:
    std::unique_ptr<dmInterface::HTTPContentFetcherInterface> create(const std::string& url) override;
};

}  // namespace libcurlUtils
}  // namespace utils
}  // namespace aisdk

#endif  // _LIBCURLUTILS_HTTPCONTENTFETCHERFACTORY_H_