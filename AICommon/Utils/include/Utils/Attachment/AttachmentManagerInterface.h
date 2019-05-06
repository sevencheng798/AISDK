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

#ifndef __UTILS_ATTACHMENT_ATTACHMENTMANAGERINTERFACE_H_
#define __UTILS_ATTACHMENT_ATTACHMENTMANAGERINTERFACE_H_

#include <chrono>
#include <string>
#include <memory>

#include "Utils/Attachment/Attachment.h"
#include "Utils/Attachment/AttachmentReader.h"
#include "Utils/Attachment/AttachmentWriter.h"

namespace aisdk {
namespace utils {
namespace attachment {

/**
 * This class allows the decoupling of attachment readers and writers from the management of attachments.
 *
 * This class is thread safe.
 */
class AttachmentManagerInterface {
public:
    /**
     * Destructor.
     */
    virtual ~AttachmentManagerInterface() = default;

    /**
     * Returns a pointer to an @c AttachmentWriter.
     * @note Calls to @c createReader and @c createWriter may occur in any order.
     *
     * @param attachmentId The id of the @c Attachment.
     * @param policy The WriterPolicy that the AttachmentWriter should adhere to.
     * @return An @c AttachmentWriter.
     */
    virtual std::unique_ptr<AttachmentWriter> createWriter(
        const std::string& attachmentId,
        utils::sharedbuffer::WriterPolicy policy = utils::sharedbuffer::WriterPolicy::NONBLOCKABLE) = 0;

    /**
     * Returns a pointer to an @c AttachmentReader.
     * @note Calls to @c createReader and @c createWriter may occur in any order.
     *
     * @param attachmentId The id of the @c Attachment.
     * @param policy The @c AttachmentReader policy, which determines the semantics of the @c AttachmentReader.read().
     * @return An @c AttachmentReader.
     */
    virtual std::unique_ptr<AttachmentReader> createReader(
        const std::string& attachmentId,
        utils::sharedbuffer::ReaderPolicy policy) = 0;
};

}  // namespace attachment
}  // namespace utils
}  // namespace aisdk

#endif  // __UTILS_ATTACHMENT_ATTACHMENTMANAGERINTERFACE_H_