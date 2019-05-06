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
 
#include <iostream>

#include "Utils/SafeShutdown.h"

namespace aisdk {
namespace utils {
	
SafeShutdown::SafeShutdown(const std::string& name) : m_name{name}, m_isShutdown{false} {
	
}

SafeShutdown::~SafeShutdown() {
	if (!m_isShutdown) {
		std::cout << "~RequiresShutdownFailed:reason:notShutdown:name: " << m_name << std::endl;
	}

	std::cout << __func__ << " comein..." << std::endl;
}

const std::string& SafeShutdown::name() const {
	return m_name;
}

void SafeShutdown::shutdown() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_isShutdown) {
		std::cout << "shutdownFailed:reason:alreadyShutdown:name: " << m_name << std::endl;
		return;
	}
	doShutdown();
	m_isShutdown = true;
}

bool SafeShutdown::isShutdown() const {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_isShutdown;
}

}	//utils
} // namespace aisdk
