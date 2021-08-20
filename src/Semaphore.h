#pragma once

#include "Helpers.h"

#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    explicit Semaphore() = default;

    void release() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_locked = false;
        m_condition.notify_one();
    }

    void acquire() {
        std::unique_lock<std::mutex> lock(m_mutex);
        // handle spurious wake-ups
        while (m_locked) {
            m_condition.wait(lock);
        }

        m_locked = true;
    }

    bool try_acquire() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_locked) {
            m_locked = true;
            return true;
        }

        return false;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;

    // initialize as unlocked
    bool m_locked = false;
};