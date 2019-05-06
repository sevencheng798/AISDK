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

#ifndef __UTILS_ATTACHMENT_INPROCESSATTACHMENT_H_
#define __UTILS_ATTACHMENT_INPROCESSATTACHMENT_H_

#include "Utils/Attachment/Attachment.h"
#include "Utils/Attachment/InProcessAttachmentReader.h"
#include "Utils/Attachment/InProcessAttachmentWriter.h"
#include "Utils/Threading/Memory.h" // Support make_unique

#include "Utils/SharedBuffer/SharedBuffer.h"

namespace aisdk {
namespace utils {
namespace attachment {

/**
 * A class that represents an AVS attachment following an in-process memory management model.
 */
class InProcessAttachment : public Attachment {
public:
    /// Type aliases for convenience.
    using SDSType = utils::sharedbuffer::SharedBuffer;
    using SDSBufferType = SDSType::Buffer;

    /// Default size of underlying SDS when created internally.
    static const int SDS_BUFFER_DEFAULT_SIZE_IN_BYTES = 0x500000;

    /**
     * Constructor.
     *
     * @param id The attachment id.
     * @param sds The underlying @c SharedBuffer object.  If not specified, then this class will create its own.
     */
    InProcessAttachment(const std::string& id, std::unique_ptr<SDSType> sds = nullptr);

    std::unique_ptr<AttachmentWriter> createWriter(
        InProcessAttachmentWriter::SDSTypeWriter::Policy policy =
            InProcessAttachmentWriter::SDSTypeWriter::Policy::NONBLOCKABLE) override;

    std::unique_ptr<AttachmentReader> createReader(InProcessAttachmentReader::SDSTypeReader::Policy policy) override;

private:
    // The sds from which we will create the reader and writer.
    std::shared_ptr<SDSType> m_sds;
};

}  // namespace attachment
}  // namespace utils
}  // namespace aisdk

#endif  // __UTILS_ATTACHMENT_INPROCESSATTACHMENT_H_