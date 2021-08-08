#pragma once

#include "Semaphore.h"

#include <functional>
#include <mutex>

class RelayThread {
public:
    explicit RelayThread();

    ~RelayThread();

    void run(std::function<void()>&& f, std::function<void()>&& done);

    bool lock();

private:
    std::function<void()> m_function;
    std::function<void()> m_done;
    std::thread m_thread;

    Semaphore m_threadLock;
    Semaphore m_runningLock;

    bool m_exit = false;
};
