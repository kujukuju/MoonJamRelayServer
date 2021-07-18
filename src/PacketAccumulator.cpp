#include "PacketAccumulator.h"

#include "RoomManager.h"

PacketAccumulator::PacketAccumulator(RoomManager &roomManager)
        : m_roomManager(roomManager) {

}

bool PacketAccumulator::hasRoom(std::array<char, HASH_LENGTH> room) {
    const std::lock_guard<std::mutex> packetLock(m_packetMutex);

    return m_packets.contains(room);
}

void PacketAccumulator::addPacket(std::array<char, HASH_LENGTH> room, ReceivedPacket packet) {
    const std::lock_guard<std::mutex> packetLock(m_packetMutex);

    if (!m_packets.contains(room)) {
        return;
    }

    std::vector<ReceivedPacket>& roomPackets = m_packets[room];
    roomPackets.emplace_back(packet);
}

void PacketAccumulator::createRoom(std::array<char, HASH_LENGTH> room) {
    {
        const std::lock_guard<std::mutex> packetLock(m_packetMutex);
        if (m_packets.contains(room)) {
            return;
        }

        m_packets[room] = std::vector<ReceivedPacket>();
    }

    m_roomManager.createRoom(*this, room);
}

void PacketAccumulator::destroyRoom(std::array<char, HASH_LENGTH> room) {
    const std::lock_guard<std::mutex> packetLock(m_packetMutex);
    if (!m_packets.contains(room)) {
        return;
    }

    m_packets.erase(m_packets.find(room));
}
