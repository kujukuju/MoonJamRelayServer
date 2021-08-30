#include "RelayThreadPool.h"

RelayThreadController RelayThreadPool::run(std::function<void()>&& function) {
    RelayThreadController controller;

    // TODO could optimize this with conditional notify unique lock stuff
    while (true) {
        for (RelayThread& thread : m_threads) {
            if (thread.lock()) {
                controller.valid = true;
                controller.mutex->acquire();
                thread.run(std::move(function), [controller]() {
                    controller.mutex->release();
                });

                return controller;
            }
        }
    }
}
