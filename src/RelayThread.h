#pragma once

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

    std::mutex m_threadLock;
    std::mutex m_runningLock;

    bool m_exit = false;
};
