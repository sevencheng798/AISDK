/*
 * Copyright 2019 gm its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */


#include "Utils/Threading/Memory.h"
#include "Utils/Threading/Executor.h"

namespace aisdk {
namespace utils {
namespace threading {

Executor::Executor() :
        m_taskQueue{std::make_shared<TaskQueue>()},
        m_taskThread{memory::make_unique<TaskThread>(m_taskQueue)} {
    m_taskThread->start();
}

Executor::~Executor() {
    shutdown();
}

void Executor::waitForSubmittedTasks() {
#if 0
    std::promise<void> flushedPromise;
    auto flushedFuture = flushedPromise.get_future();
    auto task = [&flushedPromise]() { flushedPromise.set_value(); };
    submit(task);
    flushedFuture.get();
#endif
}

void Executor::shutdown() {
    m_taskQueue->shutdown();
    m_taskThread.reset();
}

bool Executor::isShutdown() {
    return m_taskQueue->isShutdown();
}

}  // namespace threading
}  // namespace utils
}  // namespace aisdk
