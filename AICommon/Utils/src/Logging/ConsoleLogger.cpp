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

#include <cstdio>
#include <ctime>
#include <iostream>
#include <mutex>

#include "Utils/Logging/ConsoleLogger.h"
#include "Utils/Logging/ThreadMoniker.h"

namespace aisdk {
namespace utils {
namespace logging {

/// Configuration key for DefaultLogger settings
static const std::string CONFIG_KEY_DEFAULT_LOGGER = "consoleLogger";

std::shared_ptr<Logger> ConsoleLogger::instance() {
    static std::shared_ptr<Logger> singleConsoletLogger = std::shared_ptr<ConsoleLogger>(new ConsoleLogger);
    return singleConsoletLogger;
}

void ConsoleLogger::emit(
    Level level,
    std::chrono::system_clock::time_point time,
    const char* threadMoniker,
    const char* text) {
    std::lock_guard<std::mutex> lock(m_coutMutex);
    std::cout << m_logFormatter.format(level, time, threadMoniker, text) << std::endl;
}

ConsoleLogger::ConsoleLogger() : Logger(Level::UNKNOWN) {
#ifdef DEBUG
	// default DEBUG0 or DEBUG5
    setLevel(Level::DEBUG5);
#else
    setLevel(Level::INFO);
#endif  // DEBUG
   // init(configuration::ConfigurationNode::getRoot()[CONFIG_KEY_DEFAULT_LOGGER]);
    std::string ver{"v1.0.1"};
    std::string currentVersionLogEntry("sdkVersion: " + ver);  //Sven
    emit(
        aisdk::utils::logging::Level::INFO,
        std::chrono::system_clock::now(),
        "1",
        currentVersionLogEntry.c_str());
	//ThreadMoniker::getThisThreadMoniker().c_str(),  fix log issue Sven
}

std::shared_ptr<Logger> getConsoleLogger() {
    return ConsoleLogger::instance();
}

}  // namespace logging
}  // namespace utils
}  // namespace aisdk
