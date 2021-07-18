#include "RoomManager.h"

#include "PacketAccumulator.h"
#include "RelayServer.h"

static const long long TICKRATE = 30;

void RoomManager::createRoom(PacketAccumulator& packetAccumulator, std::array<char, HASH_LENGTH> room) {
    const std::lock_guard<std::mutex> roomLock(m_roomMutex);

    if (m_roomThreads.contains(room)) {
        return;
    }

    m_roomThreads[room] = std::thread([this, &packetAccumulator, room] {
        std::vector<connection_hdl> connections;
        std::vector<ReceivedPacket> packets;
        std::vector<uint8_t> bytes;

        while (packetAccumulator.hasRoom(room)) {
            auto startTime = std::chrono::high_resolution_clock::now();

            // do stuff
            // for now just send back all packets? no filtering?
            packetAccumulator.getConnections(connections, room);
            packetAccumulator.getPackets(packets, room);

            int byteCount = 0;
            for (auto& packet : packets) {
                byteCount += packet.length;
            }
            bytes.resize(byteCount);

            int currentIndex = 0;
            for (auto& packet : packets) {
                std::memcpy(bytes.data() + currentIndex, packet.bytes.data(), packet.length);
                currentIndex += packet.length;
            }

            for (auto& connection : connections) {
                RelayServer::send(connection, bytes.data(), bytes.size());
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
            long long delay = std::max(0LL, 1000000000LL / TICKRATE - duration);

            std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
        }

        // can you do this from within the thread that will be destroyed as a result of this action?
        const std::lock_guard<std::mutex> roomLock(m_roomMutex);
        m_roomThreads.erase(room);
    });
}
