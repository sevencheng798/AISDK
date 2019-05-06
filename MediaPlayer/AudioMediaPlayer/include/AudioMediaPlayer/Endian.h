/*
 * Copyright 2018 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef __UTILS_ENDIAN_H_
#define __UTILS_ENDIAN_H_

#include <cstdint>

namespace aisdk {
namespace mediaPlayer {
namespace ffmpeg {

/// Function used to check the machine endianness.
inline bool littleEndianMachine() {
    uint16_t original = 1;
    uint8_t* words = reinterpret_cast<uint8_t*>(&original);
    return words[0] == 1;
}

}  // namespace ffmpeg
}  // namespace mediaPlayer
} //namespace aisdk
#endif  // __UTILS_ENDIAN_H_