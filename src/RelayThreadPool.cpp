#include "RelayThreadPool.h"

RelayThreadPool::RelayThreadPool()
        : m_queued(0) {

}

RelayThreadController RelayThreadPool::run(std::function<void()>&& function) {
    RelayThreadController controller;

    for (RelayThread& thread : m_threads) {
        if (thread.lock()) {
            controller.valid = true;
            controller.mutex->acquire();
            thread.run(std::move(function), [this, controller]() {
                controller.mutex->release();
                processQueue();
            });

            return controller;
        }
    }

    m_queued.push(std::move(function));

    return controller;
}

void RelayThreadPool::processQueue() {
    if (m_queued.empty()) {
        return;
    }

    for (RelayThread& thread : m_threads) {
        if (thread.lock()) {
            thread.run(std::move(m_queued.pop()), [this]() {
                processQueue();
            });

            if (m_queued.empty()) {
                return;
            }
        }
    }
}
