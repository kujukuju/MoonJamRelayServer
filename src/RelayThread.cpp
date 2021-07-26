#include "RelayThread.h"

#include <iostream>

RelayThread::RelayThread()
        : m_done([]() {}),
          m_threadLock(1),
          m_runningLock(1) {
    m_threadLock.acquire();

    m_thread = std::thread([this] {
        while (!m_exit) {
            m_runningLock.release();
            m_runningLock.acquire();

            // allow this thread to be owned by another process
            m_threadLock.release();

            m_done();

            // you must call lock to claim ownership then call run to unlock runningLock and get past this line
            m_runningLock.acquire();

            // if runningLock was released by the deconstructor then exit will be true and we back out to be joined
            if (m_exit) {
                break;
            }

            m_function();
        }
    });
}

RelayThread::~RelayThread() {
    // wait for the current execution to finish... is this allowed in the deconstructor? it delays deconstruction
    m_threadLock.acquire();

    // set exit to true before unlocking the runningLock to continue
    m_exit = true;

    // allow the thread to continue to the exit condition
    m_runningLock.release();

    // join the thread to get rid of it or whatever
    m_thread.join();
}

void RelayThread::run(std::function<void()>&& f, std::function<void()>&& done) {
    m_function = std::move(f);
    m_done = std::move(done);
    m_runningLock.release();
}

/// This function tries to lock the thread and returns whether or not it was successful. If it returns true it is
/// required that you immediately call the run method.
bool RelayThread::lock() {
    if (m_threadLock.try_acquire()) {
        return true;
    }

    return false;
}
