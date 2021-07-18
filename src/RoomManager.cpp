#include "RoomManager.h"

#include "PacketAccumulator.h"

static const long long TICKRATE = 30;

void RoomManager::createRoom(PacketAccumulator& packetAccumulator, std::array<char, HASH_LENGTH> room) {
    const std::lock_guard<std::mutex> roomLock(m_roomMutex);

    if (m_roomThreads.contains(room)) {
        return;
    }

    m_roomThreads[room] = std::thread([this, &packetAccumulator, room] {
        while (packetAccumulator.hasRoom(room)) {
            auto startTime = std::chrono::high_resolution_clock::now();

            // do stuff
            // for now just send back all packets? no filtering?


            auto endTime = std::chrono::high_resolution_clock::now();
            long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
            long delay = std::max(0LL, 1000000000L / TICKRATE - duration);

            std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
        }

        // can you do this from within the thread that will be destroyed as a result of this action?
        const std::lock_guard<std::mutex> roomLock(m_roomMutex);
        m_roomThreads.erase(room);
    });
}
