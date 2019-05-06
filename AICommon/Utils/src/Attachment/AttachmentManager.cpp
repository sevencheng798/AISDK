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

#include <vector>

#include "Utils/Attachment/InProcessAttachment.h"
#include "Utils/Logging/Logger.h"
#include "Utils/Threading/Memory.h"

#include "Utils/Attachment/AttachmentManager.h"

namespace aisdk {
namespace utils {
namespace attachment {

/// String to identify log entries originating from this file.
static const std::string TAG("AttachmentManager");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) utils::logging::LogEntry(TAG, event)

// The definition for these two static class members.
constexpr std::chrono::minutes AttachmentManager::ATTACHMENT_MANAGER_TIMOUT_MINUTES_DEFAULT;

AttachmentManager::AttachmentManagementDocker::AttachmentManagementDocker() :
        creationTime{std::chrono::steady_clock::now()} {
}

AttachmentManager::AttachmentManager() :
        m_attachmentExpirationMinutes{ATTACHMENT_MANAGER_TIMOUT_MINUTES_DEFAULT} {
}

AttachmentManager::AttachmentManagementDocker& AttachmentManager::getDockersLocked(const std::string& attachmentId) {
    // This call ensures the dockers object exists, whether updated previously, or as a new object.
    auto& dockers = m_attachmentDockersMap[attachmentId];

    // If it's a new object, the inner attachment has not yet been created.  Let's go do that.
    if (!dockers.attachment) {
        dockers.attachment = memory::make_unique<InProcessAttachment>(attachmentId);
        if (!dockers.attachment) {
            AISDK_ERROR(LX("getDetailsLockedError").d("reason", "Unsupported attachment type"));
        }
    }

    return dockers;
}

std::unique_ptr<AttachmentWriter> AttachmentManager::createWriter(
    const std::string& attachmentId,
    utils::sharedbuffer::WriterPolicy policy) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto& dockers = getDockersLocked(attachmentId);
    if (!dockers.attachment) {
        AISDK_ERROR(LX("createWriterFailed").d("reason", "Could not access attachment"));
        return nullptr;
    }

    auto writer = dockers.attachment->createWriter(policy);
    removeExpiredAttachmentsLocked();
    return writer;
}

std::unique_ptr<AttachmentReader> AttachmentManager::createReader(
    const std::string& attachmentId,
    utils::sharedbuffer::ReaderPolicy policy) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto& dockers = getDockersLocked(attachmentId);
    if (!dockers.attachment) {
        AISDK_ERROR(LX("createWriterFailed").d("reason", "Could not access attachment"));
        return nullptr;
    }

    auto reader = dockers.attachment->createReader(policy);
    removeExpiredAttachmentsLocked();
    return reader;
}

void AttachmentManager::removeExpiredAttachmentsLocked() {
    std::vector<std::string> idsToErase;
    auto now = std::chrono::steady_clock::now();

    for (auto& iter : m_attachmentDockersMap) {
        auto& dockers = iter.second;

        /*
         * Our criteria for releasing an AttachmentManagementDockers object - either:
         *  - Both futures have been returned, which means the attachment now has a reader and writer.  Great!
         *  - Only the reader or writer future was returned, and the attachment has exceeded its lifetime limit.
         */

        auto attachmentLifetime = std::chrono::duration_cast<std::chrono::minutes>(now - dockers.creationTime);

        if ((dockers.attachment->hasCreatedReader() && dockers.attachment->hasCreatedWriter()) ||
            attachmentLifetime > m_attachmentExpirationMinutes) {
            idsToErase.push_back(iter.first);
        }
    }

    for (auto id : idsToErase) {
        m_attachmentDockersMap.erase(id);
    }
}

}  // namespace attachment
}  // namespace utils
}  // namespace aisdk
