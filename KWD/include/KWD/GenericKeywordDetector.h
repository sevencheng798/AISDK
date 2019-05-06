/*
 * Copyright 2019 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef __GENERIC_KEYWORDDETECTOR_H_
#define __GENERIC_KEYWORDDETECTOR_H_

#include <unordered_set>

#include <Utils/SharedBuffer/BufferLayout.h>
#include <DMInterface/KeyWordObserverInterface.h>

namespace aisdk {
namespace kwd {

class GenericKeywordDetector {
public:
    /**
     * Adds the specified observer to the list of observers to notify of key word detection events.
     *
     * @param keyWordObserver The observer to add.
     */
    void addKeyWordObserver(std::shared_ptr<dmInterface::KeyWordObserverInterface> keyWordObserver);

    /**
     * Removes the specified observer to the list of observers to notify of key word detection events.
     *
     * @param keyWordObserver The observer to remove.
     */
    void removeKeyWordObserver(std::shared_ptr<dmInterface::KeyWordObserverInterface> keyWordObserver);

    /**
     * Destructor.
     */
    virtual ~GenericKeywordDetector() = default;

protected:
    /**
     * Constructor.
     *
     * @param keyWordObservers The observers to notify of keyword detections.
     */
    GenericKeywordDetector(
        std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>> keyWordObservers =
            std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>>());

    /**
     * Notifies all keyword observers of the keyword detection.
     *
     * @param stream The stream in which the keyword was detected.
     * @param keyword The keyword detected.
     * @param beginIndex The absolute begin index of the first part of the keyword found within the @c stream.
     * @param endIndex The absolute end index of the last part of the keyword within the stream of the @c stream.
     * @param KWDMetadata Wake word engine metadata.
     */
    void notifyKeyWordObservers(
        std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
        std::string keyword,
        utils::sharedbuffer::SharedBuffer::Index beginIndex = dmInterface::KeyWordObserverInterface::UNSPECIFIED_INDEX,
        utils::sharedbuffer::SharedBuffer::Index endIndex = dmInterface::KeyWordObserverInterface::UNSPECIFIED_INDEX) const;

    /**
     * Reads from the specified stream into the specified buffer and does the appropriate error checking and observer
     * notifications.
     *
     * @param reader The stream reader. This should be a blocking reader.
     * @param buf The buffer to read into.
     * @param nWords The number of words to read.
     * @param timeout The amount of time to wait for data to become available.
     * @param[out] errorOccurred Lets caller know if there were any errors that occurred with the read call.
     * @return The number of words successfully read.
     */
    ssize_t readFromStream(
        std::shared_ptr<utils::sharedbuffer::Reader> reader,
        std::shared_ptr<utils::sharedbuffer::SharedBuffer> stream,
        void* buf,
        size_t nWords,
        std::chrono::milliseconds timeout,
        bool* errorOccurred);

    /**
     * Checks to see if the @c audioFormat matches the platform endianness.
     *
     * @param audioFormat The audio format to check
     * @return @c true if the endiannesses don't match and @c false otherwise.
     */
    static bool isByteswappingRequired();

private:
    /**
     * The observers to notify on key word detections. This should be locked with m_keyWordObserversMutex prior to
     * usage.
     */
    std::unordered_set<std::shared_ptr<dmInterface::KeyWordObserverInterface>> m_keyWordObservers;

    /// Lock to protect m_keyWordObservers when users wish to add or remove observers
    mutable std::mutex m_keyWordObserversMutex;

};

}  // namespace kwd
}  // namespace aisdk

#endif  // __GENERIC_KEYWORDDETECTOR_H_