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
#ifndef __ASR_TIMERER_H_
#define __ASR_TIMERER_H_
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <thread>

namespace aisdk {
namespace asr {

class ASRTimer {
public:
	static const size_t FOREVER = 0;
	
	/// Constructor
	ASRTimer();

	///
	template <typename Rep, typename Period, typename Task, typename... Args>
    auto start(const std::chrono::duration<Rep, Period>& delay, Task task, Args&&... args)
        -> std::future<decltype(task(args...))>;

	void stop();

	bool isActive() const;

	~ASRTimer();
private:
	bool activate();
	
    template <typename Rep, typename Period>
    void callTask(
        std::chrono::duration<Rep, Period> delay,
        std::chrono::duration<Rep, Period> period,
        size_t maxCount,
        std::function<void()> task);

    /// The condition variable used to wait for @c stop() or period timeouts.
    std::condition_variable m_waitCondition;

    /// The mutex for @c m_waitCondition.
    std::mutex m_waitMutex;

	/// The thread to execute tasks on.
    std::thread m_thread;

    /// Flag which indicates that a @c Timer is active.
    std::atomic<bool> m_running;

    /**
     * Flag which requests that the active @c Timer be stopped.  @c m_waitMutex must be locked when modifying this
     * variable.
     */
    bool m_stopping;
};

template <typename Rep, typename Period, typename Task, typename... Args>
auto ASRTimer::start(const std::chrono::duration<Rep, Period>& delay, Task task, Args&&... args)
	-> std::future<decltype(task(args...))> {
	// Can't start if already running.
	if(!activate()) {
		using FutureType = decltype(task(args...));
		return std::future<FutureType>();	// vaild = false;
	}

	// Join old timer thread (if any).
	if(m_thread.joinable()) {
		m_thread.join();
	}

	// Remove arguments from the task's type by binding the arguments to the task.
    auto boundTask = std::bind(std::forward<Task>(task), std::forward<Args>(args)...);
	
    /*
     * Create a std::packaged_task with the correct return type. The decltype only returns the return value of the
     * boundTask. The following parentheses make it a function call with the boundTask return type. The package task
     * will then return a future of the correct type.
     */
    using PackagedTaskType = std::packaged_task<decltype(boundTask())()>;
    auto packagedTask = std::make_shared<PackagedTaskType>(boundTask);

    // Remove the return type from the task by wrapping it in a lambda with no return value.
    auto translatedTask = [packagedTask]() { packagedTask->operator()(); };

    // Kick off the new timer thread.
    static const size_t once = 1;
    m_thread = std::thread{
        std::bind(&ASRTimer::callTask<Rep, Period>, this, delay, delay, once, translatedTask)};

    return packagedTask->get_future();
}

template <typename Rep, typename Period>
void ASRTimer::callTask(
    std::chrono::duration<Rep, Period> delay,
    std::chrono::duration<Rep, Period> period,
    size_t maxCount,
    std::function<void()> task) {
    // Timepoint to measure delay/period against.
    auto now = std::chrono::steady_clock::now();

    // Flag indicating whether we've drifted off schedule.
    bool offSchedule = false;

    for (size_t count = 0; maxCount == FOREVER || count < maxCount; ++count) {
        auto waitTime = (0 == count) ? delay : period;
        {
            std::unique_lock<std::mutex> lock(m_waitMutex);

            // Wait for stop() or a delay/period to elapse.
            if (m_waitCondition.wait_until(lock, now + waitTime, [this]() { return m_stopping; })) {
                m_stopping = false;
                m_running = false;
                return;
            }
        }
		
		// Update our estimate of where we should be after the delay.
		now += waitTime;
		
		// Run the task if we're still on schedule.
		if (!offSchedule) {
			task();
		}
		
		// If the task runtime put us off schedule, skip the next task run.
		if (now + period < std::chrono::steady_clock::now()) {
			offSchedule = true;
		} else {
			offSchedule = false;
		}

    }
    m_stopping = false;
    m_running = false;
}


}	//asr
} // namespace aisdk
#endif //__ASR_TIMERER_H_
