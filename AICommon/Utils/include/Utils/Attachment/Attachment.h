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

#ifndef __UTILS_ATTACHMENT_H_
#define __UTILS_ATTACHMENT_H_

#include <atomic>
#include <mutex>
#include <string>

#include "Utils/Attachment/AttachmentReader.h"
#include "Utils/Attachment/AttachmentWriter.h"
#include "Utils/SharedBuffer/SharedBuffer.h"
#include "Utils/SharedBuffer/ReaderPolicy.h"
#include "Utils/SharedBuffer/WriterPolicy.h"

namespace aisdk {
namespace utils {
namespace attachment {

/**
 * A class that represents an NLP 'text to speech' attachment.
 */
class Attachment {
public:
    /**
     * Constructor.
     *
     * @param attachmentId The id for this attachment.
     */
    Attachment(const std::string& attachmentId);

    /**
     * Destructor.
     */
    virtual ~Attachment() = default;

    /**
     * Creates a writer object, with which the Attachment may be written to.
     *
     * @return a @unique_ptr to an AttachmentWriter.
     */

    virtual std::unique_ptr<AttachmentWriter> createWriter(
        utils::sharedbuffer::WriterPolicy policy = utils::sharedbuffer::WriterPolicy::NONBLOCKABLE) = 0;

    /**
     * Creates a reader object, with which the Attachment may be read from.
     *
     * @param The policy used to configure the reader.
     * @return a @unique_ptr to an AttachmentReader.
     * */
    virtual std::unique_ptr<AttachmentReader> createReader(utils::sharedbuffer::ReaderPolicy policy) = 0;

    /**
     * An accessor to get the attachmentId.
     *
     * @return The attachment Id.
     */
    std::string getId() const;

    /**
     * Utility function to tell if a reader has been created for this object.
     *
     * @return Whether a reader has been created for this object.
     */
    bool hasCreatedReader();

    /**
     * Utility function to tell if a writer has been created for this object.
     *
     * @return Whether a writer has been created for this object.
     */
    bool hasCreatedWriter();

protected:
    /// The id for this attachment object.
    const std::string m_id;
    /// mutex to protext access to the createReader and createWriter API.
    std::mutex m_mutex;
    /// An atomic tracking variable to tell whether this object has created a writer.
    std::atomic<bool> m_hasCreatedWriter;
    /// An atomic tracking variable to tell whether this object has created a reader.
    std::atomic<bool> m_hasCreatedReader;
};

}  // namespace attachment
}  // namespace utils
}  // namespace aisdk

#endif  // __UTILS_ATTACHMENT_H_