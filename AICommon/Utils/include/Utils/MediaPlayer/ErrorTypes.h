/*
 * Copyright 2018 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef __AICOMMON_UTILS_INCLUDE_MEDIAPLAYER_ERRORTYPES_H_
#define __AICOMMON_UTILS_INCLUDE_MEDIAPLAYER_ERRORTYPES_H_

#include <ostream>

namespace aisdk {
namespace utils {
namespace mediaPlayer {

/// Identifies the specific type of error in a @c PlaybackFailed event.
enum class ErrorType {
    /// An unknown error occurred.
    MEDIA_ERROR_UNKNOWN,
    
    /// The server recognized the request as being malformed (bad request, unauthorized, forbidden, not found, etc).
    MEDIA_ERROR_INVALID_REQUEST,
    
    /// The client was unable to reach the service.
    MEDIA_ERROR_SERVICE_UNAVAILABLE,
    
    /// The server accepted the request, but was unable to process the request as expected.
    MEDIA_ERROR_INTERNAL_SERVER_ERROR,
    
    /// There was an internal error on the client.
    MEDIA_ERROR_INTERNAL_DEVICE_ERROR
};

/**
 * Convert an @c ErrorType to an pa-compliant @c std::string.
 *
 * @param errorType The @c ErrorType to convert.
 * @return The PA-compliant string representation of @c errorType.
 */
inline std::string errorTypeToString(ErrorType errorType) {
    switch (errorType) {
        case ErrorType::MEDIA_ERROR_UNKNOWN:
            return "MEDIA_ERROR_UNKNOWN";
        case ErrorType::MEDIA_ERROR_INVALID_REQUEST:
            return "MEDIA_ERROR_INVALID_REQUEST";
        case ErrorType::MEDIA_ERROR_SERVICE_UNAVAILABLE:
            return "MEDIA_ERROR_SERVICE_UNAVAILABLE";
        case ErrorType::MEDIA_ERROR_INTERNAL_SERVER_ERROR:
            return "MEDIA_ERROR_INTERNAL_SERVER_ERROR";
        case ErrorType::MEDIA_ERROR_INTERNAL_DEVICE_ERROR:
            return "MEDIA_ERROR_INTERNAL_DEVICE_ERROR";
    }
    return "unknown ErrorType";
}

/*
 * 为什么不把output运算符设计为一个member function呢？
 * 因为作为一个member function，其左侧操作数必须是隶属同一个class之下的对象，
 * 如果output运算符被设计为menber function，那么该类的对象必须被置于output运算符的左侧： 
 *	object << cout << endl;
*/

/**
 * Write an @c ErrorType value to an @c ostream.
 *
 * @param stream The stream to write the value to.
 * @param errorType The @c ErrorType value to write to the @c ostream as a string.
 * @return The @c ostream that was passed in and written to.
 */
inline std::ostream& operator<<(std::ostream& stream, const ErrorType& errorType) {
    return stream << errorTypeToString(errorType);
}

}  // namespace mediaPlayer
}  // namespace utils
} //namespace aisdk
#endif  // _AICOMMON_UTILS_INCLUDE_MEDIAPLAYER_ERRORTYPES_H_
