#include "PacketAccumulator.h"

#include "RoomManager.h"

PacketAccumulator::PacketAccumulator(RoomManager &roomManager)
        : m_roomManager(roomManager) {

}

bool PacketAccumulator::hasRoom(std::array<char, HASH_LENGTH> room) {
    const std::lock_guard<std::mutex> packetLock(m_packetMutex);

    return m_packets.contains(room);
}

void PacketAccumulator::addPacket(std::array<char, HASH_LENGTH> room, ReceivedPacket packet, connection_hdl&& handle) {
    {
        const std::lock_guard<std::mutex> packetLock(m_packetMutex);

        if (!m_packets.contains(room)) {
            return;
        }

        std::vector<ReceivedPacket>& roomPackets = m_packets[room];
        roomPackets.emplace_back(packet);
    }

    const std::lock_guard<std::mutex> handleLock(m_handleMutex);
    std::vector<connection_hdl>& roomHandles = m_handles[room];
    auto it = std::find_if(roomHandles.begin(), roomHandles.end(), [&handle](const auto& entry) {
        return handle.lock().get() == entry.lock().get();
    });

    if (it == roomHandles.end()) {
        roomHandles.emplace_back(handle);
    }
}

void PacketAccumulator::removeConnection(connection_hdl&& handle) {
    const std::lock_guard<std::mutex> handleLock(m_handleMutex);

    // technically you could be connected to more than one room, so we check all
    // later we can optimize this using another map going the opposite direction
    for (auto& room : m_handles) {
        auto it = std::find_if(room.second.begin(), room.second.end(), [&handle](const auto& entry) {
            return handle.lock().get() == entry.lock().get();
        });

        if (it != room.second.end()) {
            room.second.erase(it);
        }
    }
}

void PacketAccumulator::createRoom(std::array<char, HASH_LENGTH> room) {
    {
        const std::lock_guard<std::mutex> packetLock(m_packetMutex);
        if (m_packets.contains(room)) {
            return;
        }

        m_packets[room] = std::vector<ReceivedPacket>();
    }

    {
        const std::lock_guard<std::mutex> handleLock(m_handleMutex);
        m_handles[room] = std::vector<connection_hdl>();
    }

    m_roomManager.createRoom(*this, room);
}

void PacketAccumulator::destroyRoom(std::array<char, HASH_LENGTH> room) {
    {
        const std::lock_guard<std::mutex> packetLock(m_packetMutex);
        if (!m_packets.contains(room)) {
            return;
        }

        m_packets.erase(m_packets.find(room));
    }

    const std::lock_guard<std::mutex> handleLock(m_handleMutex);
    m_handles.erase(room);
}

void PacketAccumulator::getConnections(std::vector<connection_hdl>& connections, std::array<char, HASH_LENGTH> room) {
    connections.clear();

    const std::lock_guard<std::mutex> handleLock(m_handleMutex);

    if (!m_handles.contains(room)) {
        return;
    }

    std::vector<connection_hdl>& existing = m_handles[room];
    connections.reserve(existing.size());
    std::copy(existing.begin(), existing.end(), std::back_inserter(connections));
}

void PacketAccumulator::getPackets(std::vector<ReceivedPacket>& packets, std::array<char, HASH_LENGTH> room) {
    packets.clear();

    const std::lock_guard<std::mutex> packetLock(m_packetMutex);
    if (!m_packets.contains(room)) {
        return;
    }

    std::vector<ReceivedPacket>& roomPackets = m_packets[room];
    packets.swap(roomPackets);
}
