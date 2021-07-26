#pragma once

#include "Helpers.h"
#include "RelayThreadPool.h"

#include <unordered_map>
#include <thread>
#include <mutex>
#include <vector>

class PacketAccumulator;

class RoomManager {
public:
    explicit RoomManager() = default;

    void createRoom(PacketAccumulator& packetAccumulator, std::array<char, HASH_LENGTH> room);

private:
    std::mutex m_roomMutex;
    std::unordered_map<std::array<char, HASH_LENGTH>, std::thread> m_roomThreads;

    std::mutex m_completedMutex;
    std::vector<std::array<char, HASH_LENGTH>> m_completedThreads;

    RelayThreadPool m_threadPool;
};
