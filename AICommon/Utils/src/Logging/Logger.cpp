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

#include <algorithm>
#include <chrono>
#include "Utils/Logging/Logger.h"
#include "Utils/Logging/ThreadMoniker.h"

namespace aisdk {
namespace utils {
namespace logging {

/// Configuration key for root level "logger" object.
static const std::string CONFIG_KEY_LOGGER = "logger";

/// Configuration key for "logLevel" values under "logger" and other per-module objects.
static const std::string CONFIG_KEY_LOG_LEVEL = "logLevel";

static constexpr auto AT_EXIT_THREAD_ID = "0";

Logger::Logger(Level level) : m_level{level} {
}

void Logger::log(Level level, const LogEntry& entry) {
    if (shouldLog(level)) {
        emit(level, std::chrono::system_clock::now(), AT_EXIT_THREAD_ID, entry.c_str());	// fix issue log sven
    }
}

void Logger::setLevel(Level level) {
    // notify observers of logLevel changes
    if (m_level != level) {
        m_level = level;
        //notifyObserversOnLogLevelChanged();
    }
#ifndef AISDK_DEBUG_LOG_ENABLED
    if (m_level <= Level::DEBUG0) {
        // Log without AISDK_* macros to avoid recursive invocation of constructor.
        log(Level::WARN,
            LogEntry("Logger", "debugLogLevelSpecifiedWhenDebugLogsCompiledOut")
                .d("level", m_level)
                .m("\n"
                   "\nWARNING: By default DEBUG logs are compiled out of RELEASE builds."
                   "\nRebuild with the cmake parameter -DCMAKE_BUILD_TYPE=DEBUG to enable debug logs."
                   "\n"));
    }
#endif
}

void Logger::logAtExit(Level level, const LogEntry& entry) {
    if (shouldLog(level)) {
        emit(level, std::chrono::system_clock::now(), AT_EXIT_THREAD_ID, entry.c_str());
    }
}

}  // namespace logging
}  // namespace utils
}  // namespace aisdk
