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

#ifndef _TASK_THREADING_H_
#define _TASK_THREADING_H_

#include <atomic>
#include <memory>
#include <thread>

#include "TaskQueue.h"

namespace aisdk {
namespace utils {
namespace threading {

/**
 * A TaskThread is a thread which reads from a TaskQueue and executes those tasks.
 */
class TaskThread {
public:
    /**
     * Constructs
     *
     * @params taskQueue A TaskQueue to take tasks from to execute.
     */
    TaskThread(std::shared_ptr<TaskQueue> taskQueue);

    /**
     * Destructs the TaskThread.
     */
    ~TaskThread();

    /**
     * Starts executing tasks from the queue on the thread.
     */
    void start();

    /**
     * Whether or not the TaskThread has been shutdown.
     *
     * @returns whether or not the TaskThread has been shutdown.
     */
    bool isShutdown();

private:
    /**
     * Loops over.
     */
    void processTasksLoop();

    /// A weak pointer to the TaskQueue
    std::weak_ptr<TaskQueue> m_taskQueue;

    /// A flag to message the task thread to stop executing.
    std::atomic_bool m_shutdown;

    /// The thread to run tasks on.
    std::thread m_thread;
};

}  // namespace threading
}  // namespace utils
}  // namespace aisdk

#endif  // _TASK_THREADING_H_