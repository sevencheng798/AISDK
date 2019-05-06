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
 
#ifndef __BUFFER_LAYOUT_H_
#define __BUFFER_LAYOUT_H_
#include <string>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
class BufferLayout {
public:
	using Buffer = std::vector<uint8_t>;
	using Index = uint64_t;

    /**
     * The constructor only initializes a shared pointer to the provided buffer.  Attaching and/or initializing is
     * performed by the @c init()/@c attach() functions.
     *
     * @param buffer The raw buffer which holds (or will hold) the header, arrays, and circular data buffer.
     */
    BufferLayout(std::shared_ptr<Buffer> buffer);

    /// The destructor ensures the BufferLayout is @c detach()es from the Buffer.
    ~BufferLayout();
	/**
     * This structure defines the header fields for the @c Buffer.
     */
    struct Header {
        /**
         * This field specifies the word size (in bytes).
         */
        uint16_t wordSize;

        /**
         * This field specifies the maximum number of @c Readers.
         */
        uint8_t maxReaders;

        /// This field contains the condition variable used to notify @c Readers that data is available.
        std::condition_variable dataAvailableConditionVariable;

        /// This field contains the mutex used by @c dataAvailableConditionVariable.
        std::mutex dataAvailableMutex;

        /**
         * This field contains the condition variable used to notify @c Writers that space is available.  Note that
         * this condition variable does not have a dedicated mutex; the condition is protected by backwardSeekMutex.
         */
        std::condition_variable spaceAvailableConditionVariable;

        /**
         * This field contains a mutex used to temporarily hold off @c Readers from seeking backwards in the buffer
         * while a @c Reader is updating @c oldestUnconsumedCursor.
         */
        std::mutex backwardSeekMutex;

        /// This field indicates whether there is an enabled (not closed) @c Writer.
        std::atomic<bool> isWriterEnabled;

        /// This field indicates that a @c Writer had at one point been enabled and then closed.
        std::atomic<bool> hasWriterBeenClosed;

        /**
         * This mutex is used to protect creation of the writer.  In particular, it is locked when attempting to add
         * the writer so that there are no races between overlapping calls to @c createWriter().
         */
        std::mutex writerEnableMutex;

        /// This field contains the next location to write to.
        std::atomic<uint64_t> writeStartCursor;

        /**
         * This field contains the end of the region currently being written to (when no write is in progress,
         * `(writEndCursor == writeStartCursor)`).
         */
        std::atomic<uint64_t> writeEndCursor;

        /// This field tracks the number of BufferLayout instances currently attached to a Buffer.
        uint32_t referenceCount;

        std::mutex attachMutex;

        /**
         * This mutex is used to protect creation of readers.  In particular, it is locked when attempting to add a new
         * reader so that there are no races between overlapping calls to @c createReader().
         */
        std::mutex readerEnableMutex;
    };

    // return A pointer to the array of @c maxReaders cursor @c Indexes.
    std::atomic<bool>* getReaderEnabledArray() const;

    // return A pointer to the array of @c maxReaders cursor @c Indexes.
    std::atomic<uint64_t>* getReaderCursorArray() const;

    // return A pointer to the array of @c maxReaders close @c Indexes.
    std::atomic<uint64_t>* getReaderCloseIndexArray() const;
	
    /**
     * This function provides access to the Header structure stored at the start of the @c Buffer.
     *
     * @return A pointer to the @c Header structure inside the @c Buffer.
     */
    Header* getHeader() const;

	// return The maximum number of words(non-bytes) the stream can store.
	Index getDataSize() const;

	/**
	 * This function provides access to the data (non-Header) portion of @c buffer.
	 * @params at An optional word @c Index to get a data pointer for. 
	 * @return A pointer to the data which would hold the specified @c Index.
	 */
	uint8_t* getData(Index at = 0) const;

	bool init(size_t wordSize, size_t maxReaders);

	//bool attach();

	void detach();

	bool isReaderEnabled(size_t id) const;
	/**
     * This function enables the specified reader.  The caller must be holding Header::readerEnableMutex when calling
     * this function.
     *
     * @param id The id of the reader to enable.
     */
    void enableReaderLocked(size_t id);

    /**
     * This function disables the specified reader.  The caller must be holding Header::readerEnableMutex when calling
     * this function.
     *
     * @param id The id of the reader to disable.
     */
    void disableReaderLocked(size_t id);

    /**
     * This function returns a count of the number of words after the specified @c Index before the circular data
     * will wrap.
     *
     * @c param after The @c Index to count from.
     * @c return The count of words after @c after until the circular data will wrap.
     */
    Index wordsUntilWrap(Index after) const;
    /** 
     * This function calculates the offset (in bytes) from the start of a @c Buffer to the start of the circular data
     * (start storing the start address of Raw Data or (header + allReaderArray)).
     *
     * @param wordSize The size (in bytes) of words in the stream.
     * @param maxReaders The maximum number of readers the stream will support.
     * @return The offset (in bytes) from the start of a @c Buffer to the start of the circular data.
     */
    static size_t calculateDataOffset(size_t wordSize, size_t maxReaders);
private:
	/**
     * This function rounds @c size up to a multiple of @c align.
     *
     * @param size The size to round up.
     * @param align The alignment to round to.
     * @return @c size rounded up to a multiple of @c align.
     */
    static size_t alignSizeTo(size_t size, size_t align);

    /**
     * This function calculates the offset (in bytes) from the start of a @c Buffer to the start of the @c Reader
     * enabled array.
     *
     * @return The offset (in bytes) from the start of a @c Buffer.
     */
    static size_t calculateReaderEnabledArrayOffset();

    /**
     * This function calculates the offset (in bytes) from the start of a @c Buffer to the start of the @c Reader
     * cursor array.
     *
     * @param maxReaders The maximum number of readers the stream will support.
     * @return The offset (in bytes) from the start of a @c Buffer to the start of the @c Reader cursor array.
     */
    static size_t calculateReaderCursorArrayOffset(size_t maxReaders);

    /**
     * This function calculates the offset (in bytes) from the start of a @c Buffer to the start of the @c Reader
     * close @c Index array.
     *
     * @param maxReaders The maximum number of readers the stream will support.
     * @return The offset (in bytes) from the start of a @c Buffer to the start of the @c Reader close @c Index array.
     */
    static size_t calculateReaderCloseIndexArrayOffset(size_t maxReaders);

    /**
     * This function calculates several frequently-accessed constants and caches them in member variables.
     *
     * @param wordSize The size (in bytes) of words in the stream.  All @c SharedDataStream operations that work with
     *     data or position in the stream are quantified in words.
     * @param maxReaders The maximum number of readers the stream will support.
     */
    void calculateAndCacheConstants(size_t wordSize, size_t maxReaders);

	// return true if the @c SharedBuffer is attached to its @c Buffer, else false 
	bool isAttached() const;
	/**
	 * Circular buffer @c m_buffer storage format:
	 * | ---  |	------------------ 	 ------------------   ---------------------- |  ------------- |
	 * |Header|n*ReaderEnabledArray|+n*ReaderCursorArray|+n*ReaderCloseIndexArray|raw data stream |
	 * | ---  |	------------------   ------------------	  ---------------------- |  ------------- |
	*/
    /// The @c Buffer used to store the stream's header and data.
    std::shared_ptr<Buffer> m_buffer;

    /// Precalculated pointer to the @c Reader enabled array.
    std::atomic<bool>* m_readerEnabledArray;

    /// Precalculated pointer to the @c Reader cursor array.
    std::atomic<uint64_t>* m_readerCursorArray;

    /// Precalculated pointer to the @c Reader close @c Index array.
    std::atomic<uint64_t>* m_readerCloseIndexArray;

    /// Precalculated size (in words) of the circular data.
    Index m_dataSize;

    /// Precalculated pointer to the circular data.
    uint8_t* m_data;
};

}	// namespace sharedbuffer
}	//utils
} // namespace aisdk

#endif //__BUFFER_LAYOUT_H_