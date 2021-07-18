#pragma once

#include "Helpers.h"

#include <unordered_map>
#include <thread>
#include <mutex>

class PacketAccumulator;

class RoomManager {
public:
    explicit RoomManager() = default;

    void createRoom(PacketAccumulator& packetAccumulator, std::array<char, HASH_LENGTH> room);

private:
    std::mutex m_roomMutex;
    std::unordered_map<std::array<char, HASH_LENGTH>, std::thread> m_roomThreads;
};
