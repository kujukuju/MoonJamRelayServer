#pragma once

#include "RelayThread.h"
#include "ContiguousQueue.h"
#include "Helpers.h"

#include <array>
#include <thread>
#include <functional>

static const int THREAD_COUNT = 1024;

struct RelayThreadController {
    bool valid;
    std::shared_ptr<Semaphore> mutex = std::make_shared<Semaphore>();

    void join() const {
        mutex->acquire();
        mutex->release();
    }
};

class RelayThreadPool {
public:
    explicit RelayThreadPool() = default;

    RelayThreadController run(std::function<void()>&& function);

private:
    std::array<RelayThread, THREAD_COUNT> m_threads;
};
