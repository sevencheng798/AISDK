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
#include <iostream>
#include <ao/ao.h> 
#include "AudioMediaPlayer/AOEngine.h"


namespace aisdk {
namespace mediaPlayer {
namespace ffmpeg {

std::atomic_flag AOEngine::m_created = ATOMIC_FLAG_INIT;

std::shared_ptr<AOEngine> AOEngine::create() {

	if(!m_created.test_and_set()) {
		/// Initialize.
		ao_initialize();

		// Setup for default driver.
		int default_driver = ao_default_driver_id();
		if(default_driver < 0) {
			std::cout << "createFailed:reason=setupForDefaultDriverFailed." << std::endl;
			return nullptr;
		}
		
		return std::shared_ptr<AOEngine>(new AOEngine(default_driver));
	}

	return nullptr;
}

AOEngine::AOEngine(int defaultDriver):m_defaultDriver{defaultDriver} {

}

AOEngine::~AOEngine() {
	ao_shutdown();
	m_created.clear();
}

int AOEngine::getDefaultDriver() {
	return m_defaultDriver;
}

}  // namespace ffmpeg
}  // namespace mediaPlayer
} //namespace aisdk
