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

#ifndef __LOGGER_CONSOLELOGGER_H_
#define __LOGGER_CONSOLELOGGER_H_

#include "Utils/Logging/Logger.h"
#include "Utils/Logging/LogStringFormatter.h"

namespace aisdk {
namespace utils {
namespace logging {

/**
 * A very simple (e.g. not asynchronous) @c Logger that logs to console.
 *
 * Inheriting @c std::ios_base::Init ensures that the standard iostreams objects are properly initialized before @c
 * ConsoleLogger uses them.
 */
class ConsoleLogger
        : public Logger
        , private std::ios_base::Init {
public:
    /**
     * Return the one and only @c ConsoleLogger instance.
     *
     * @return The one and only @c ConsoleLogger instance.
     */
    static std::shared_ptr<Logger> instance();

    void emit(Level level, std::chrono::system_clock::time_point time, const char* threadMoniker, const char* text)
        override;

private:
    /**
     * Constructor.
     */
    ConsoleLogger();

    std::mutex m_coutMutex;

    /// Object to format log strings correctly.
    LogStringFormatter m_logFormatter;
};

/**
 * Return the singleton instance of @c ConsoleLogger.
 *
 * @return The singleton instance of @c ConsoleLogger.
 */
std::shared_ptr<Logger> getConsoleLogger();

}  // namespace logging
}  // namespace utils
}  // namespace aisdk

#endif  // __LOGGER_CONSOLELOGGER_H_