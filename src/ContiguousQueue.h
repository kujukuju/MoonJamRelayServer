#pragma once

#include <vector>
#include <cassert>
#include <iostream>

template<typename T>
class ContiguousQueue {
public:
    explicit ContiguousQueue() : ContiguousQueue(INITIAL_QUEUE_SIZE) {

    }

    explicit ContiguousQueue(long long size) : m_queued(0) {
        resize(size);
    }

    T operator[] (long long index) const {
        return m_queued[index].entry;
    }

    T& operator[] (long long index) {
        return m_queued[index].entry;
    }

    T next() const {
        return m_queued[m_firstIndex].entry;
    }

    T& next() {
        return m_queued[m_firstIndex].entry;
    }

    T pop() {
        T entry = std::move(m_queued[m_firstIndex].entry);
        remove(m_firstIndex);

        return entry;
    }

    void push(T&& entry) {
        if (m_queued.empty()) {
            resize(INITIAL_QUEUE_SIZE);
        }

        // if the current index isn't active just reuse it
        if (!m_queued[m_currentIndex].active) {
            m_queued[m_currentIndex].active = true;
            m_queued[m_currentIndex].entry = std::move(entry);
            m_activeCount++;
            return;
        }

        std::cout << "m_currentIndex " << m_currentIndex << std::endl;
        std::cout << "m_queued[m_currentIndex].active " << m_queued[m_currentIndex].active << std::endl;

        if (m_queued[m_currentIndex].nextIndex == -1) {
            resize(m_queued.size() * 2);
        }

        // next index shouldn't be -1 after resizing
        assert(m_queued[m_currentIndex].nextIndex != -1);
        m_currentIndex = m_queued[m_currentIndex].nextIndex;

        // the available index shouldn't be active
        assert(m_queued[m_currentIndex].active == false);
        m_queued[m_currentIndex].active = true;
        m_queued[m_currentIndex].entry = std::move(entry);
        m_activeCount++;
    }

    void remove(long long index) {
        long long lastIndex = m_queued[index].lastIndex;
        long long nextIndex = m_queued[index].nextIndex;
        if (lastIndex >= 0) {
            m_queued[lastIndex].nextIndex = nextIndex;
        }
        if (nextIndex >= 0) {
            m_queued[nextIndex].lastIndex = lastIndex;
        }

        long long oldNextIndex = m_queued[m_currentIndex].nextIndex;
        m_queued[m_currentIndex].nextIndex = index;
        m_queued[index].lastIndex = m_currentIndex;
        m_queued[index].nextIndex = oldNextIndex;
        m_queued[index].active = false;
        m_activeCount--;

        if (index == m_firstIndex) {
            if (m_activeCount > 0) {
                // if we're freeing the first index then by nature of how the free occurs the next index should be active
                assert(m_queued[nextIndex].active == true);
                m_firstIndex = nextIndex;
            } else {
                m_firstIndex = m_currentIndex;
            }
        }
    }

    long long reserved() const {
        return m_queued.size();
    }

    long long size() const {
        return m_activeCount;
    }

    bool empty() const {
        return m_activeCount == 0;
    }

private:
    struct QueuedEntry {
        long long lastIndex;
        long long nextIndex;
        bool active;
        T entry;
    };

    static const int INITIAL_QUEUE_SIZE = 10;

    void resize(long long size) {
        if (size <= m_queued.size()) {
            return;
        }

        // you can only resize if you're on the last index and it doesn't have a next entry
        std::cout << "m_currentIndex " << m_currentIndex << std::endl;
        if (!m_queued.empty() && m_queued[m_currentIndex].nextIndex != -1) {
            return;
        }

        int startIndex = m_queued.size();
        m_queued.resize(size);

        for (int i = startIndex; i < m_queued.size(); i++) {
            if (i == startIndex) {
                m_queued[i].lastIndex = -1;
            } else {
                m_queued[i].lastIndex = i - 1;
            }

            if (i == m_queued.size() - 1) {
                m_queued[i].nextIndex = -1;
            } else {
                m_queued[i].nextIndex = i + 1;
            }

            m_queued[i].active = false;
        }

        // if there was previously allocated capacity point it to the new parts
        if (startIndex > 0) {
            m_queued[m_currentIndex].nextIndex = startIndex;
            m_queued[startIndex].lastIndex = m_currentIndex;
        }
    }

    long long m_activeCount = 0;
    long long m_firstIndex = 0;
    long long m_currentIndex = 0;
    std::vector<QueuedEntry> m_queued;
};
