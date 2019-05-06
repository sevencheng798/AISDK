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

#ifndef __LOGGER_LOGGER_H_
#define __LOGGER_LOGGER_H_

#include <atomic>
#include <chrono>
#include <mutex>
#include <sstream>
#include <vector>

#include "Utils/Logging/Level.h"
#include "Utils/Logging/LogEntry.h"

/**
 * Inner part of ACSDK_STRINGIFY.  Turns an expression in to a string literal.
 *
 * @param expression The expression to turn in to a string literal.
 */
#define ACSDK_STRINGIFY_INNER(expression) #expression

/**
 * Turn a macro in to a string literal.
 *
 * @param macro The macro to expand and turn in to a string literal.
 */
#define ACSDK_STRINGIFY(macro) ACSDK_STRINGIFY_INNER(macro)

/**
 * Inner part of ACSDK_CONCATENATE.  Concatenate two expressions in to a token.
 *
 * @param lhs The expression to turn in to the left hand part of the token.
 * @param rhs The expression to turn in to the right hand part of the token.
 */
#define ACSDK_CONCATENATE_INNER(lhs, rhs) lhs##rhs

/**
 * Concatenate two macros in to a token.
 *
 * @param lhs The macro to turn in to the left hand part of the token.
 * @param rhs The macro to turn in to the right hand part of the token.
 */
#define ACSDK_CONCATENATE(lhs, rhs) ACSDK_CONCATENATE_INNER(lhs, rhs)

namespace aisdk {
namespace utils {
namespace logging {

/**
 * @c Logger provides an interface for capturing log entries as well as some core logging functionality.
 * This includes:
 * - Capturing the time, thread ID, and severity level to associate with each log entry.
 * - Accessors for a @c logLevel value that specifies the minimum severity level of a log entries that will be emitted.
 * - Initialization of logging parameters (so far just @c logLevel) from a @c ConfigurationNode.
 *
 * The @c Logger interface is not typically used directly.  Instead, calls to it are usually wrapped in
 * invocations of macros.  These macros provide a way to selectively compile out logging code.  They also
 * provide additional contextual information and direct logs to the appropriate @c Logger instance.
 *
 * Typically, each .cpp source file using @c Logger defines a constant string and a macro near the top of the file.
 * The constant string is typically named @c TAG.  It specifies the name of the @c source of log entries originating
 * from the file.  This is usually the name of the class that is implemented in the file.  The macro is typically
 * named @c LX.  It is used to create a @c LogEntry instance in-line with the expression that builds the text
 * that is to be logged.  The resulting @c LogEntry instance is passed to whatever instance of @c Logger is in use
 * in the file.  @c LX bakes in @c TAG to associate the log entry with its source.  This macro also takes an
 * argument that specifies the name of the @c event that is being logged.  Both the constant string and event name
 * are passed to the @c LogEntry constructor.  Here is an example of the definitions that typically appear at
 * the start of a .cpp file:
 *
 *     static const std::string TAG = "MyClass";
 *     #define LX(event) aisdk::utils::logger::LogEntry(TAG, event)
 *
 * When an event is to be logged, a wrapper macro named @c ACDK_<LEVEL> is invoked with an expression that starts
 * with an invocation of the @c LX macro.  The value of <LEVEL> is the name of the @c LogLevel value to associate
 * with the @c LogEntry.  Here is an example of a very simple log line that logs a "somethingHappened" event from
 * the source @c TAG with a severity level of @c INFO.
 *
 *     AISDK_INFO(LX("somethingHappened"));
 *
 * The @c LogEntry class has a builder style method that provides a convenient way to parameterize the event with
 * named properties.  This is done by adding zero or more @c LogEntry::d() method invocations after the
 * @c LX() expression.  The name of the property is always a string, and the value can be of any type for which
 * an @c operator<<(std::ostream&, <type>) has been defined. These pairs are appended to the accumulating log
 * string.  Here is an example log line with two parameters:
 *
 *     AISDK_WARN(LX("weirdnessHappened").d("<param1Name>", "stringValue").d("<param2Name>", 1 + 1 + 1);
 *
 * The @c LogEntry class also has a rarely used @c m() method that can be used to add an optional free-form
 * string at the end of the @c LogEntry.  When present, this must be the last method appended to the expression.
 * Here is an example of a log line with one parameter and a free-form message:
 *
 *     AISDK_WARN(LX("weirdnessHappened").d("<param1Name>", "stringValue").m("free form text at the end");
 *
 * The @c AISDK_<LEVEL> macros allow logs to be selectively eliminated from the generated code.  By default
 * logs of severity @c DEBUG0 and above are included in @c DEBUG builds, and logs of severity
 * @c INFO and above are in included in non @c DEBUG builds.  These macros also perform an in-line @c logLevel
 * check before evaluating the @c LX() expression.  That allows much of the CPU overhead of compiled-in log
 * lines to be selectively bypassed at run-time if the @c Logger's log level is set to not emit them.
 *
 */
class Logger {
public:
    /**
     * Logger constructor.
     *
     * @param level The lowest severity level of logs to be emitted by this Logger.
     */
    Logger(Level level);

    /// Destructor.
    virtual ~Logger() = default;

    /**
     * Set the lowest severity level to be output by this logger.
     *
     * @param level The lowest severity level to be output by this logger.
     */
    virtual void setLevel(Level level);

    /**
     * Return true of logs of a specified severity should be emitted by this Logger.
     *
     * @param level The Level to check.
     * @return Returns true if logs of the specified Level should be emitted.
     */
    inline bool shouldLog(Level level) const;

    /**
     * Send a log entry to this Logger.
     *
     * @param level The severity Level to associate with this log entry.
     * @param entry Object used to build the text of this log entry.
     */
    void log(Level level, const LogEntry& entry);

    /**
     * Send a log entry to this Logger while program is exiting.
     *
     * Use this method if the code may be run while destroying a static object. This method should not rely in any
     * other static object.
     *
     * @note The user code should still ensure that the Logger object itself is valid.
     *
     * @param level The severity Level to associate with this log entry.
     * @param entry Object used to build the text of this log entry.
     */
    void logAtExit(Level level, const LogEntry& entry);

    /**
     * Emit a log entry.
     * NOTE: This method must be thread-safe.
     * NOTE: Delays in returning from this method may hold up calls to Logger::log().
     *
     * @param level The severity Level of this log line.
     * @param time The time that the event to log occurred.
     * @param threadMoniker Moniker of the thread that generated the event.
     * @param text The text of the entry to log.
     */
    virtual void emit(
        Level level,
        std::chrono::system_clock::time_point time,
        const char* threadMoniker,
        const char* text) = 0;

protected:

    /// The lowest severity level of logs to be output by this Logger.
    std::atomic<Level> m_level;
};

bool Logger::shouldLog(Level level) const {
    return level >= m_level;
}

/**
 * Macro for building function name of the form get<type>Logger().
 *
 * @param type The type of @c Logger for which to build a get<type>Logger() function name.
 */
#define ACSDK_GET_LOGGER_FUNCTION_NAME(type) ACSDK_CONCATENATE(ACSDK_CONCATENATE(get, type), Logger)

// If @c ACSDK_LOG_SINK was not defined, default to logging to console.
#ifndef ACSDK_LOG_SINK
#define ACSDK_LOG_SINK Console
#endif

/// Build the get<type>Logger function name for whatever @c Logger logs will be sent to.
#define ACSDK_GET_SINK_LOGGER ACSDK_GET_LOGGER_FUNCTION_NAME(ACSDK_LOG_SINK)

/**
 * Get the @c Logger that logs should be sent to.
 *
 * @return The @c Logger that logs should be sent to.
 */
std::shared_ptr<Logger> ACSDK_GET_SINK_LOGGER();

}  // namespace logging
}  // namespace utils
}  // namespace aisdk

#ifdef ACSDK_LOG_MODULE

#else  // ACSDK_LOG_MODULE

namespace aisdk {
namespace utils {
namespace logging {

/**
 * Inline method to get the function that ACSDK_<LEVEL> macros will send logs to.
 * In this case @c ACSDK_LOG_MODULE was not defined, so logs are sent to the @c Logger returned by
 * @c get<ACSDK_LOG_SINK>Logger().
 */
inline Logger& ACSDK_GET_LOGGER_FUNCTION() {
    static std::shared_ptr<Logger> logger = ACSDK_GET_SINK_LOGGER();
    return *logger;
}

}  // namespace logging
}  // namespace utils
}  // namespace aisdk

#endif

/**
 * Common implementation for sending entries to the log.
 *
 * @param level The log level to associate with the log line.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define ACSDK_LOG(level, entry)                                                                       \
    do {                                                                                              \
        auto& loggerInstance = aisdk::utils::logging::ACSDK_GET_LOGGER_FUNCTION(); \
        if (loggerInstance.shouldLog(level)) {                                                        \
            loggerInstance.log(level, entry);                                                         \
        }                                                                                             \
    } while (false)

#ifdef AISDK_DEBUG_LOG_ENABLED

/**
 * Send a DEBUG5 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG5(entry) ACSDK_LOG(aisdk::utils::logging::Level::DEBUG5, entry)

/**
 * Send a DEBUG4 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG4(entry) ACSDK_LOG(aisdk::utils::logging::Level::DEBUG4, entry)

/**
 * Send a DEBUG3 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG3(entry) ACSDK_LOG(aisdk::utils::logging::Level::DEBUG3, entry)

/**
 * Send a DEBUG2 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG2(entry) ACSDK_LOG(aisdk::utils::logging::Level::DEBUG2, entry)

/**
 * Send a DEBUG1 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG1(entry) ACSDK_LOG(aisdk::utils::logging::Level::DEBUG1, entry)

/**
 * Send a DEBUG0 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG0(entry) ACSDK_LOG(aisdk::utils::logging::Level::DEBUG0, entry)

/**
 * Send a log line at the default debug level (DEBUG0).
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG(entry) ACSDK_LOG(aisdk::utils::logging::Level::DEBUG0, entry)

#else  // AISDK_DEBUG_LOG_ENABLED

/**
 * Compile out a DEBUG5 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG5(entry)

/**
 * Compile out a DEBUG4 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG4(entry)

/**
 * Compile out a DEBUG3 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG3(entry)

/**
 * Compile out a DEBUG2 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG2(entry)

/**
 * Compile out a DEBUG1 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG1(entry)

/**
 * Compile out a DEBUG0 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG0(entry)

/**
 * Compile out a DEBUG severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_DEBUG(entry)

#endif  // AISDK_DEBUG_LOG_ENABLED

/**
 * Send a INFO severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_INFO(entry) ACSDK_LOG(aisdk::utils::logging::Level::INFO, entry)

/**
 * Send a WARN severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_WARN(entry) ACSDK_LOG(aisdk::utils::logging::Level::WARN, entry)

/**
 * Send a ERROR severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_ERROR(entry) ACSDK_LOG(aisdk::utils::logging::Level::ERROR, entry)
/**
 * Send a CRITICAL severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define AISDK_CRITICAL(entry) ACSDK_LOG(aisdk::utils::logging::Level::CRITICAL, entry)

#endif  // __LOGGER_LOGGER_H_
