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
 
#ifndef __WRITER_BUFFER_H_
#define __WRITER_BUFFER_H_

#include <Utils/SharedBuffer/BufferLayout.h>
#include <Utils/SharedBuffer/WriterPolicy.h>

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
class Writer {
public:
	/// Specifies the policy to use for writing to the stream.
    using Policy = WriterPolicy;

    // Enumerates error codes which may be returned by @c write().
    struct Error {
        enum {
            /// Returned when @c close() has been previously called on the @c Writer.
            CLOSED = 0,
            /// Returned when the @c write() would overwrrite unconsumed data.
            WOULDBLOCK = -1,
            /// Returned when a @c write() parameter is invalid.
            INVALID = -2,
            /// Returned when policy is @c Policy::BLOCKING and no space becomes available before the specified timeout.
            TIMEDOUT = -3,
        };
    };

    /**
     * Constructs a new @c Writer.
     *
     * @param policy The policy to use for reading from the stream.
     * @param stream The @c BufferLayout to use for writing stream data.
     */
    Writer(Policy policy, std::shared_ptr<BufferLayout> bufferLayout);

    /// This destructor detaches the @c Writer from a @c BufferLayout.
    ~Writer();

    /**
     * This function adds new data to the stream by copying it from the provided buffer.
     *
     * @param buf A buffer to copy the data from.
     * @param nWords The maximum number of @c wordSize words to copy.
     * @param timeout The maximum time to wait (if @c policy is @c BLOCKING) for space to write into.  If this parameter
     *     is zero, there is no timeout and blocking writes will wait forever.  If @c policy is not @C BLOCKING, this
     *     parameter is ignored.
     * @return The number of @c wordSize words copied, or zero if the stream has closed, or a
     *     negative @c Error code if the stream is still open, but no data could be written.
     */
	ssize_t write(const void* buf, size_t nWords, std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

    /**
     * This function reports the current position of the @c Writer in the stream.
     *
     * @return The @c Writer's position (in @c wordSize words) in the stream.
     */
    BufferLayout::Index tell() const;

    /**
     * This function closes the @c Writer.
     */
    void close();

    /**
     * This function returns the word size (in bytes). The number(bytes) of each word.
     *
     * @return The size (in bytes) of words.
     */
    size_t getWordSize() const;
#if 0
    /**
     * Returns the text of an error code.
     *
     * @return Text of the specified error.
     */
    static std::string errorToString(Error error);
#endif
private:
	/// The @c Policy to use for writing to the stream.
    Policy m_policy;
	
    /// The @c BufferLayout of the shared buffer.
    std::shared_ptr<BufferLayout> m_bufferLayout;

	/// A flags indicating whether this writer has closed.
    bool m_closed;
};

}	// namespace sharedbuffer
}	//utils
} // namespace aisdk

#endif //__SHARED_BUFFER_H_
