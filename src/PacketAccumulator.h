#pragma once

#include <Helpers.h>

#include <unordered_map>
#include <array>
#include <mutex>
#include <websocketpp/server.hpp>

using websocketpp::connection_hdl;

static const int MAX_PACKET_SIZE = 16384;

struct ReceivedPacket {
    uint16_t identifier;
    std::array<uint8_t, MAX_PACKET_SIZE> bytes;
    uint16_t length;
};

struct ReceivedConnection {
    uint16_t identifier;
    connection_hdl handle;
};

class RoomManager;

class PacketAccumulator {
public:
    explicit PacketAccumulator(RoomManager& roomManager);

    bool hasRoom(std::array<char, HASH_LENGTH> room);

    void addPacket(std::array<char, HASH_LENGTH> room, ReceivedPacket packet, connection_hdl&& handle);

    void removeConnection(uint16_t identifier);

    void createRoom(std::array<char, HASH_LENGTH> room);

    void destroyRoom(std::array<char, HASH_LENGTH> room);

    void getConnections(std::vector<ReceivedConnection>& connections, std::array<char, HASH_LENGTH> room);

    void getPackets(std::vector<ReceivedPacket>& packets, std::array<char, HASH_LENGTH> room);

private:
    // indexed by moons key
    std::mutex m_packetMutex;
    std::unordered_map<std::array<char, HASH_LENGTH>, std::vector<ReceivedPacket>> m_packets;

    std::mutex m_handleMutex;
    std::unordered_map<std::array<char, HASH_LENGTH>, std::vector<ReceivedConnection>> m_handles;

    RoomManager& m_roomManager;
};
