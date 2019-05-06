/*
 * Copyright 2019 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef _STDCPP11_MEMORY_H_
#define _STDCPP11_MEMORY_H_

#include <memory>
#include <utility>

namespace aisdk {
namespace utils {
namespace memory {

// https://zh.cppreference.com/w/cpp/memory/unique_ptr/make_unique
// START Herb Sutter code sample adaptation
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
// END Herb Sutter code sample adaptation

}  // namespace memory
}  // namespace utils
}  // namespace aisdk

#endif  // _STDCPP11_MEMORY_H_