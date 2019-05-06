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
 
#ifndef __SHARED_BUFFER_H_
#define __SHARED_BUFFER_H_

#include <Utils/SharedBuffer/BufferLayout.h>
#include <Utils/SharedBuffer/Writer.h>
#include <Utils/SharedBuffer/Reader.h>

namespace aisdk {
namespace utils {
namespace sharedbuffer {
	
class SharedBuffer {
public:
	using Buffer = std::vector<uint8_t>;
	using Index = uint64_t;

    static std::unique_ptr<SharedBuffer> create(
    std::shared_ptr<Buffer> buffer,
    size_t wordSize = 1,
    size_t maxReaders = 1);

	static size_t calculateBufferSize(size_t nWords, size_t wordSize = 1, size_t maxReaders = 1);

	/**
	 * This function reports the maximum number of readers support by the @c SharedBuffer.
	 * @return the maximum number of readers.
	 */
	size_t getMaxReaders() const;

    /**
     * This function returns the number of data words the stream is able to hold.  This function can be safely called
     * from multiple threads or processes.
     *
     * @return the number of data words the stream is able to hold.
     */
    Index getDataSize() const;

    /**
     * This function returns the size of word (in bytes).
     *
     * @return The size (in bytes) of words for this @c SharedBuffer.
     */
    size_t getWordSize() const;
	
	/**
	 * This function to create a new @c Writer to the stream. Only one @c Writer is allowed at a time. This function must
     * not be called from multiple threads or processes
     * @param policy The policy to use for writing to the stream.
     * @param forceReplacement If set to @c true, this parameter forcefully deallocates the @c Writer before allocating
     *  a new one.
     * @return a point to the @c Writer if create successful, otherwise return nullptr.
	 */
	std::unique_ptr<Writer> createWriter(Writer::Policy policy, bool forceReplacement = false);

	/**
	 * This function to create a new @c Reader to the stream.  This function can be safely called from multiple threads or processes.
	 * @param policy The policy to use for reading to the stream.
	 * @param startWithNewData If set to @c true, this parameter forcefully read from the lastest writer position.
	 * @return a point to the @c Reader if create successful, otherwise return nullptr.
	 */
	std::unique_ptr<Reader> createReader(Reader::Policy policy, bool startWithNewData = false);

	/// Destructor.
	~SharedBuffer();
private:
    /**
     * Constructs a new @c SharedBuffer using the provided @c Buffer.  The constructor does not attempt to
     * initialize or verify the contents of the @c Buffer; that functionality is implemented by @c create().
     */
	SharedBuffer(std::shared_ptr<Buffer> buffer);

	// This function adds a @c Reader to the stream using a specific id.
    std::unique_ptr<Reader> createReaderLocked(
        size_t id,
        Reader::Policy policy,
        bool startWithNewData,
        bool forceReplacement,
        std::unique_lock<std::mutex>* lock);
	
    /// The @c BufferLayout of the shared buffer.
    std::shared_ptr<BufferLayout> m_bufferLayout;
};
}	// namespace sharebuffer
}	//utils
} // namespace aisdk

#endif //__SHARED_BUFFER_H_
