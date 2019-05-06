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
#include "ASR/ASRTimer.h"

namespace aisdk {
namespace asr {

ASRTimer::ASRTimer() : m_running(false), m_stopping(false) {
}

ASRTimer::~ASRTimer() {
    stop();
}

void ASRTimer::stop() {
    {
        std::lock_guard<std::mutex> lock(m_waitMutex);
        if (m_running) {
            m_stopping = true;
        }
        m_waitCondition.notify_all();
    }

    if (std::this_thread::get_id() != m_thread.get_id() && m_thread.joinable()) {
        m_thread.join();
    }
}

bool ASRTimer::isActive() const {
    return m_running;
}

bool ASRTimer::activate() {
    return !m_running.exchange(true);
}

}	//asr
} // namespace aisdk
