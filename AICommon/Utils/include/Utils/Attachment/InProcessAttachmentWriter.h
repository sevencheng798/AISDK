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

#ifndef __UTILS_ATTACHMENT_INPROCESSATTACHMENTWRITER_H_
#define __UTILS_ATTACHMENT_INPROCESSATTACHMENTWRITER_H_

#include "Utils/SharedBuffer/SharedBuffer.h"

#include "AttachmentWriter.h"

namespace aisdk {
namespace utils {
namespace attachment {

/**
 * A class that provides functionality to write data to an @c Attachment.
 *
 * @note This class is not thread-safe beyond the thread-safety provided by the underlying SharedBuffer object.
 */
class InProcessAttachmentWriter : public AttachmentWriter {
public:
    /// Type aliases for convenience.
    using SDSType = utils::sharedbuffer::SharedBuffer;
    using SDSTypeWriter = utils::sharedbuffer::Writer;

    /**
     * Create an InProcessAttachmentWriter.
     *
     * @param sds The underlying @c SharedBuffer which this object will use.
     * @param policy The policy of the new Writer.
     * @return Returns a new InProcessAttachmentWriter, or nullptr if the operation failed.
     */
    static std::unique_ptr<InProcessAttachmentWriter> create(
        std::shared_ptr<SDSType> sds,
        SDSTypeWriter::Policy policy = SDSTypeWriter::Policy::NONBLOCKABLE);

    /**
     * Destructor.
     */
    ~InProcessAttachmentWriter();

    std::size_t write(
        const void* buf,
        std::size_t numBytes,
        WriteStatus* writeStatus,
        std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) override;

    void close() override;

protected:
    /**
     * Constructor.
     *
     * @param sds The underlying @c SharedBuffer which this object will use.
     * @param policy The policy of the new Writer.
     */
    InProcessAttachmentWriter(
        std::shared_ptr<SDSType> sds,
        SDSTypeWriter::Policy policy = SDSTypeWriter::Policy::NONBLOCKABLE);

    /// The underlying @c SharedBuffer reader.
    std::shared_ptr<SDSTypeWriter> m_writer;
};

}  // namespace attachment
}  // namespace utils
}  // namespace aisdk

#endif  // __UTILS_ATTACHMENT_INPROCESSATTACHMENTWRITER_H_