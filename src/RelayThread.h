#pragma once

#include <functional>
#include <mutex>
#include <semaphore>

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

    std::binary_semaphore m_threadLock;
    std::binary_semaphore m_runningLock;

    bool m_exit = false;
};
