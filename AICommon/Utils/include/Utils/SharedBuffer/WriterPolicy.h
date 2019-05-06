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
 
#ifndef __WRITER_POLICY_H_
#define __WRITER_POLICY_H_

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
/// Specifies the policy to use for writing to the stream.
enum class WriterPolicy {
    /**
     * A @c NONBLOCKABLE @c Writer will always write all the data provided without waiting for @c Readers to move
     * out of the way.
     */
    NONBLOCKABLE,
    /**
     * A @c BLOCKING @c Writer will wait for up to the specified timeout (or forever if `(timeout == 0)`) for space
     * to become available. 
     */
    BLOCKING
};

}	// namespace sharedbuffer
}	//utils
} // namespace aisdk

#endif //__WRITER_POLICY_H_