#pragma once

#include "RelayThread.h"
#include "ContiguousQueue.h"

#include <array>
#include <thread>
#include <functional>

static const int THREAD_COUNT = 1024;

struct RelayThreadController {
    bool valid;
    std::shared_ptr<std::binary_semaphore> mutex = std::make_shared<std::binary_semaphore>(1);

    void join() const {
        mutex->acquire();
        // TODO not even sure if I have to unlock here because it just gets thrown away
        mutex->release();
    }
};

class RelayThreadPool {
public:
    explicit RelayThreadPool();

    RelayThreadController run(std::function<void()>&& function);

private:
    void processQueue();

    std::array<RelayThread, THREAD_COUNT> m_threads;

    ContiguousQueue<std::function<void()>> m_queued;
};
