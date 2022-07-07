#include "PacketAccumulator.h"

#include "RoomManager.h"

PacketAccumulator::PacketAccumulator(RoomManager &roomManager)
        : m_roomManager(roomManager) {

}

bool PacketAccumulator::hasRoom(std::array<char, HASH_LENGTH> room) {
    const std::lock_guard<std::mutex> packetLock(m_packetMutex);

    return contains(m_packets, room);
}

void PacketAccumulator::addPacket(std::array<char, HASH_LENGTH> room, ReceivedPacket packet, connection_hdl&& handle) {
    {
        const std::lock_guard<std::mutex> packetLock(m_packetMutex);

        if (!contains(m_packets, room)) {
            return;
        }

        std::vector<ReceivedPacket>& roomPackets = m_packets[room];
        roomPackets.emplace_back(packet);
    }

    uint16_t identifier = packet.identifier;

    const std::lock_guard<std::mutex> handleLock(m_handleMutex);
    std::vector<ReceivedConnection>& roomHandles = m_handles[room];
    auto it = std::find_if(roomHandles.begin(), roomHandles.end(), [identifier](const auto& entry) {
        return identifier == entry.identifier;
    });

    if (it == roomHandles.end()) {
        roomHandles.emplace_back(ReceivedConnection {
            identifier,
            handle,
        });
    }
}

void PacketAccumulator::removeConnection(uint16_t identifier) {
    const std::lock_guard<std::mutex> handleLock(m_handleMutex);

    // technically you could be connected to more than one room, so we check all
    // later we can optimize this using another map going the opposite direction
    for (auto& room : m_handles) {
        auto it = std::find_if(room.second.begin(), room.second.end(), [identifier](const auto& entry) {
            return identifier == entry.identifier;
        });

        if (it != room.second.end()) {
            room.second.erase(it);
        }
    }
}

void PacketAccumulator::createRoom(std::array<char, HASH_LENGTH> room) {
    {
        const std::lock_guard<std::mutex> packetLock(m_packetMutex);
        if (contains(m_packets, room)) {
            return;
        }

        m_packets[room] = std::vector<ReceivedPacket>();
    }

    {
        const std::lock_guard<std::mutex> handleLock(m_handleMutex);
        m_handles[room] = std::vector<ReceivedConnection>();
    }

    m_roomManager.createRoom(*this, room);
}

void PacketAccumulator::destroyRoom(std::array<char, HASH_LENGTH> room) {
    {
        const std::lock_guard<std::mutex> packetLock(m_packetMutex);
        if (!contains(m_packets, room)) {
            return;
        }

        m_packets.erase(m_packets.find(room));
    }

    const std::lock_guard<std::mutex> handleLock(m_handleMutex);
    m_handles.erase(room);
}

void PacketAccumulator::getConnections(std::vector<ReceivedConnection>& connections, std::array<char, HASH_LENGTH> room) {
    connections.clear();

    const std::lock_guard<std::mutex> handleLock(m_handleMutex);

    if (!contains(m_handles, room)) {
        return;
    }

    std::vector<ReceivedConnection>& existing = m_handles[room];
    connections.reserve(existing.size());
    std::copy(existing.begin(), existing.end(), std::back_inserter(connections));
}

int PacketAccumulator::getConnectionCountWithoutConnection(uint16_t identifier, std::array<char, HASH_LENGTH> room) {
    const std::lock_guard<std::mutex> handleLock(m_handleMutex);

    if (!contains(m_handles, room)) {
        return 0;
    }

    auto& connectionList =  m_handles[room];
    int count = connectionList.size();
    auto it = std::find_if(connectionList.begin(), connectionList.end(), [identifier](const auto& entry) {
        return identifier == entry.identifier;
    });

    if (it != connectionList.end()) {
        count -= 1;
    }

    return count;
}

void PacketAccumulator::getPackets(std::vector<ReceivedPacket>& packets, std::array<char, HASH_LENGTH> room) {
    packets.clear();

    const std::lock_guard<std::mutex> packetLock(m_packetMutex);
    if (!contains(m_packets, room)) {
        return;
    }

    std::vector<ReceivedPacket>& roomPackets = m_packets[room];
    packets.swap(roomPackets);
}
