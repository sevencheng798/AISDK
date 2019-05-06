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
 
#ifndef __READER_BUFFER_H_
#define __READER_BUFFER_H_
#if 0
#include <cstdint>
#include <cstddef>
#include <vector>
#include <mutex>
#include <limits>
#include <cstring>
#endif
#include <Utils/SharedBuffer/BufferLayout.h>
#include <Utils/SharedBuffer/ReaderPolicy.h>

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
class Reader {
public:
	/// Specifies the policy to use for reading to the stream.
    using Policy = ReaderPolicy;

	/// Specifies a reference to measure @c seek()/@c tell()/@c close() offsets against.
    enum class Reference {
        /// The offset is from this @c Reader's current position: `(index = reader + offset)`.
        AFTER_READER,
        /// The offset is to this @c Reader's current position: `(index = reader - offset)`.
        BEFORE_READER,
        /// The offset is to the @c Writer's current position: `(index(readerCursor) = writer - offset)`.
        /// Valid data between readerCursor and writerStartCursor.
        BEFORE_WRITER,
        /// The offset is absolute: `(index = 0 + offset)`.
        ABSOLUTE
    };

    /// Enumerates error codes which may be returned by @c read().
    struct Error {
        enum {
            /**
             * Returned when the stream is closed - either due to a @c Writer::close() call and no remaining buffered
             * data, or due to a @c Reader::close() call which has reached its close index.
             */
            CLOSED = 0,
            /// Returned when the data requested has been overwritten and is invalid.
            OVERRUN = -1,
            /// Returned when policy is @c Policy::NONBLOCKING and no data is available.
            WOULDBLOCK = -2,
            /// Returned when policy is @c Policy::BLOCKING and no data becomes available before the specified timeout.
            TIMEDOUT = -3,
            /// Returned when a @c read() parameter is invalid.
            INVALID = -4
        };
    };


    /**
     * Constructs a new @c Reader.
     *
     * @param policy The policy to use for reading from the stream.
     * @param stream The @c BufferLayout to use for reading stream data.
     */
    Reader(Policy policy, std::shared_ptr<BufferLayout> bufferLayout, uint8_t id);

    /// This destructor detaches the @c Reader from a @c BufferLayout.
    ~Reader();

    /**
     * This function consumes data from the stream by copying it to the provided buffer.
     *
     * @param buf A buffer to copy the data from.
     * @param nWords The maximum number of @c wordSize words to copy.
     * @param timeout The maximum time to wait (if @c policy is @c BLOCKING) for space to write into.  If this parameter
     *     is zero, there is no timeout and blocking writes will wait forever.  If @c policy is not @C BLOCKING, this
     *     parameter is ignored.
     * @return The number of @c wordSize words copied, or zero if the stream has closed, or a
     *     negative @c Error code if the stream is still open, but no data could be written.
     */
	ssize_t read(void* buf, size_t nWords, std::chrono::milliseconds timeout = std::chrono::milliseconds(0));

    /**
     * This function moves the @c Reader to the specified location in the stream. 
     *
     * @param offset The position (in @c wordSize words) in the stream, relative to @c reference, to move the @c Reader
     *     to.
     * @param reference The position in the stream @c offset is applied to.
     * @return @c true if the specified position points at unconsumed data, else @c false.
     */
    bool seek(BufferLayout::Index offset, Reference reference = Reference::ABSOLUTE);
	

    /**
     * This function reports the current position of the @c Reader in the stream.
     *
     * @return The @c Reader's position (in @c wordSize words) in the stream.
     */
    BufferLayout::Index tell(Reference reference = Reference::ABSOLUTE) const;

    /**
     * This function closes the @c Reader.
     */
    void close(BufferLayout::Index offset = 0, Reference reference = Reference::AFTER_READER);

    /**
     * This function returns the word size (in bytes). The number(bytes) of each word.
     *
     * @return The size (in bytes) of words.
     */
    size_t getWordSize() const;

	/// Return the id assgined to @Reader.
	size_t getId() const;
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

	/// A index flags indicating which BufferLayout::getReaderCursorArray and 
	/// BufferLayout::getReaderCloseIndexArray to assigns @c Reader
	uint8_t m_id;

    /// Pointer to this reader's cursor in BufferLayout::getReaderCursorArray().
    std::atomic<uint64_t>* m_readerCursor;

    /// Pointer to this reader's close index in BufferLayout::getReaderCloseIndexArray().
    std::atomic<uint64_t>* m_readerCloseIndex;
};

}	// namespace sharedbuffer
}	//utils
} // namespace aisdk

#endif //__READER_BUFFER_H_
