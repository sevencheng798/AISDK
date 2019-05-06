/*
 * Copyright 2018 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef __AO_ENGINE_H_
#define __AO_ENGINE_H_
#include <memory>

namespace aisdk {
namespace mediaPlayer {
namespace ffmpeg {

/// Class that represents the libao engine object and interfaces.
class AOEngine {
public:
	/// Create libao object.
	static std::shared_ptr<AOEngine>create();

	int getDefaultDriver();

	/// Destructor.
	~AOEngine();
private:
	/// Construct.
	AOEngine(int defaultDriver);
	
	/// A specific default driver id.
	int m_defaultDriver;

    /// Atomic flag used to ensure there is only one @c AOEngine.
    static std::atomic_flag m_created;
};

}  // namespace ffmpeg
}  // namespace mediaPlayer
} //namespace aisdk
#endif  // __AO_ENGINE_H_