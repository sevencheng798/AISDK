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

#ifndef __UTILS_ATTACHMENT_ATTACHMENTMANAGER_H_
#define __UTILS_ATTACHMENT_ATTACHMENTMANAGER_H_

#include <mutex>
#include <unordered_map>

#include "Utils/Attachment/AttachmentManagerInterface.h"

namespace aisdk {
namespace utils {
namespace attachment {

/**
 * This class allows the decoupling of attachment readers and writers from the management of attachments.
 */
class AttachmentManager : public AttachmentManagerInterface {
public:
    /**
     * This is the default timeout value for attachments.  Any attachment which is inspected in the
     * @c removeExpiredAttachmentsLocked() call, and whose lifetime exceeds this value, will be released.
     */
    static constexpr std::chrono::minutes ATTACHMENT_MANAGER_TIMOUT_MINUTES_DEFAULT = std::chrono::hours(12);

    /**
     * Constructor.
     */
    AttachmentManager();

    std::unique_ptr<AttachmentWriter> createWriter(
        const std::string& attachmentId,
        utils::sharedbuffer::WriterPolicy policy = utils::sharedbuffer::WriterPolicy::NONBLOCKABLE) override;

    std::unique_ptr<AttachmentReader> createReader(const std::string& attachmentId, utils::sharedbuffer::ReaderPolicy policy)
        override;

private:
    /**
     * A utility structure to encapsulate an @c Attachment, its creation time, and other appropriate data fields.
     */
    struct AttachmentManagementDocker {
        /**
         * Constructor.  This sets the creationTime to the time of instantiation.
         */
        AttachmentManagementDocker();

        /// The time this structure instance was created.
        std::chrono::steady_clock::time_point creationTime;
        /// The Attachment this object is managing.
        std::unique_ptr<Attachment> attachment;
    };

    /**
     * A utility function to acquire the dockers object for an attachment being managed.  This function
     * encapsulates logic to set up the object if it does not already exist, before returning it.
     *
     * @note The class mutex @c m_mutex must be locked before calling this function.
     *
     * @param attachmentId The attachment id for the attachment docker being requested.
     * @return The attachment docker object.
     */
    AttachmentManagementDocker& getDockersLocked(const std::string& attachmentId);

    /**
     * A cleanup function, which will release an @c AttachmentManagementDockers from the map if either both a writer
     * and reader have been created, or if its lifetime has exceeded the timeout.
     * @note: @c m_mutex must be acquired before calling this function.
     */
    void removeExpiredAttachmentsLocked();

    /// The timeout in minutes.  Any attachment whose lifetime exceeds this value will be released.
    std::chrono::minutes m_attachmentExpirationMinutes;
    /// The mutex to ensure the non-static public APIs are thread safe.
    std::mutex m_mutex;
    /// The map of attachment dockers.
    std::unordered_map<std::string, AttachmentManagementDocker> m_attachmentDockersMap;
};

}  // namespace attachment
}  // namespace utils
}  // namespace aisdk

#endif  // __UTILS_ATTACHMENT_ATTACHMENTMANAGER_H_