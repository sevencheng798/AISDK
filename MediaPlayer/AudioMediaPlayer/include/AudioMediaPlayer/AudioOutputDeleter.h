/*
 * Copyright 2018 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#ifndef __AO_OUTPUT_DELETER_H_
#define __AO_OUTPUT_DELETER_H_

struct ao_device;

namespace aisdk {
namespace mediaPlayer {
namespace ffmpeg {

/**
 * Generic deleter class to free libao C structures.
 *
 * @tparam ObjectT The type of object to be deleted.
 */
template <typename ObjectT>
class AudioOutputDeleter {
public:
    void operator()(ObjectT* object);
};

/// @name libao AudioOutputDeleter specializations.
/// @{
using AOOpenLiveDeleter = AudioOutputDeleter<ao_device>;

template <>
void AudioOutputDeleter<ao_device>::operator()(ao_device* device) {
    ao_close(device);
}

/// @}

}  // namespace ffmpeg
}  // namespace mediaPlayer
} //namespace aisdk
#endif  // __AO_OUTPUT_DELETER_H_