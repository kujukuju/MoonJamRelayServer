#pragma once

#include <Helpers.h>

#include <unordered_map>
#include <array>
#include <mutex>

static const int MAX_PACKET_SIZE = 16384;

struct ReceivedPacket {
    uint16_t identifier;
    std::array<uint8_t, MAX_PACKET_SIZE> bytes;
    uint16_t length;
};

class RoomManager;

class PacketAccumulator {
public:
    explicit PacketAccumulator(RoomManager& roomManager);

    bool hasRoom(std::array<char, HASH_LENGTH> room);

    void addPacket(std::array<char, HASH_LENGTH> room, ReceivedPacket packet);

    void createRoom(std::array<char, HASH_LENGTH> room);

    void destroyRoom(std::array<char, HASH_LENGTH> room);

private:
    // indexed by moons key
    std::mutex m_packetMutex;
    std::unordered_map<std::array<char, HASH_LENGTH>, std::vector<ReceivedPacket>> m_packets;

    RoomManager& m_roomManager;
};
