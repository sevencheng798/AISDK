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
 
#ifndef __MICROPHONE_INTERFACE_H_
#define __MICROPHONE_INTERFACE_H_

namespace aisdk {
namespace utils {
namespace microphone {

// The acts as an interface to record audio input from a Microphone.
class MicrophoneInterface {
public:
    /**
     * Stops streaming from the microphone.
     *
     * @return Whether the stop was successful.
     */
    virtual bool stopStreamingMicrophoneData() = 0;

    /**
     * Starts streaming from the microphone.
     *
     * @return Whether the start was successful.
     */
    virtual bool startStreamingMicrophoneData() = 0;

    /**
     * Destructor.
     */
    virtual ~MicrophoneInterface() = default;
};

}	// namespace microphone
}	//utils
} // namespace aisdk

#endif //__MICROPHONE_INTERFACE_H_