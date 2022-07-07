#include "RelayServer.h"

#include "Helpers.h"
#include "KeyManager.h"
#include "PacketAccumulator.h"

RelayServer::RelayServer(const uint32_t port, KeyManager& keyManager, PacketAccumulator& packetAccumulator)
        : m_server(port),
          m_keyManager(keyManager),
          m_packetAccumulator(packetAccumulator) {
    m_server.setOpenListener([this](connection_hdl&& handle) {
        onOpen(std::move(handle));
    });

    m_server.setCloseListener([this](connection_hdl&& handle) {
        onClose(std::move(handle));
    });

    m_server.setMessageListener([this](connection_hdl&& handle, const std::string& message) {
        onMessage(std::move(handle), message);
    });
}

void RelayServer::run() {
    m_server.run();
}

void RelayServer::close(connection_hdl&& handle, websocketpp::close::status::value status, const char* reason) {
    if (handle.expired()) {
        return;
    }

    server::connection_type& connection = *(server::connection_type*) handle.lock().get();
    if (connection.get_state() != websocketpp::session::state::open) {
        return;
    }

    connection.close(status, reason);
}

void RelayServer::send(connection_hdl& handle, uint8_t* data, size_t size) {
    if (handle.expired()) {
        return;
    }

    server::connection_type& connection = *(server::connection_type*) handle.lock().get();
    if (connection.get_state() != websocketpp::session::state::open) {
        return;
    }

    connection.send(data, size, websocketpp::frame::opcode::value::BINARY);
}

void RelayServer::onOpen(connection_hdl&& handle) {
    connection_hdl* handlePointer = (connection_hdl*) handle.lock().get();

    claimIdentifier(handlePointer);
}

void RelayServer::onClose(connection_hdl&& handle) {
    connection_hdl* handlePointer = (connection_hdl*) handle.lock().get();
    uint16_t identifier = getIdentifier(handlePointer);

    {
        const std::lock_guard<std::mutex> identifierLock(m_identifierMutex);
        if (contains(m_moonIdentifierToRoomMap, identifier)) {
            std::array<char, HASH_LENGTH> room = m_moonIdentifierToRoomMap[identifier];
            m_moonIdentifierToRoomMap.erase(identifier);

            bool remainingMoons = false;
            for (auto& otherMoon : m_moonIdentifierToRoomMap) {
                std::array<char, HASH_LENGTH> otherRoom = otherMoon.second;
                if (otherRoom == room) {
                    remainingMoons = true;
                    break;
                }
            }
            if (!remainingMoons) {
                m_packetAccumulator.destroyRoom(room);
                std::cout << "Destroying room " << print(room) << std::endl;
            } else {
                std::cout << "Not destroying room " << print(room) << "... Other moon clients remain." << std::endl;
            }
        }
    }

    {
        const std::lock_guard<std::mutex> playerCountLock(m_playerCountMutex);
        m_packetAccumulator.removeConnection(identifier);
    }

    releaseIdentifier(handlePointer);
}

void RelayServer::onMessage(connection_hdl&& handle, const std::string& message) {
    static const std::array<char, HASH_LENGTH>& empty {};

    connection_hdl* handlePointer = (connection_hdl*) handle.lock().get();

    uint16_t identifier = getIdentifier(handlePointer);
    if (identifier == 0) {
        std::cerr << "Identifier was invalid. This should not be possible." << std::endl;
        return;
    }

    if (message.size() < HASH_LENGTH) {
        close(std::move(handle), websocketpp::close::status::internal_endpoint_error, "You must include a room specifier.");
        return;
    }

    std::array<char, HASH_LENGTH> hash {};
    std::memcpy(hash.data(), message.data(), HASH_LENGTH);

    std::array<char, HASH_LENGTH> room = m_keyManager.getRoom(hash);
    if (room == empty) {
        close(std::move(handle), websocketpp::close::status::internal_endpoint_error, "Your room specifier cannot be empty.");
        return;
    }

    // add a moon client, we allow for multiple moon clients
    bool moonClient = room == hash;
    if (moonClient) {
        const std::lock_guard<std::mutex> identifierLock(m_identifierMutex);
        m_moonIdentifierToRoomMap[identifier] = room;
    }

    // this checks that the room exists, which would be a result of moons client being connected
    if (!m_packetAccumulator.hasRoom(room)) {
        // if this client is moon, create the room
        if (moonClient) {
            // const std::lock_guard<std::mutex> identifierLock(m_identifierMutex);

            // if (m_moonIdentifierToRoomMap.contains(identifier)) {
            //     std::cout << "Destroying existing room. " << print(room) << std::endl;
            //     m_packetAccumulator.destroyRoom(room);
            // }

            std::cout << "Creating new room. " << print(room) << std::endl;
            m_packetAccumulator.createRoom(room);
        } else {
            close(std::move(handle), websocketpp::close::status::internal_endpoint_error, "Room not available. Moonmoon must connect first.");
            return;
        }
    }

    // validate the max packet size
    if (message.size() > HASH_LENGTH + MAX_PACKET_SIZE) {
        // don't recognize this message... do we send back an error?
        return;
    }

    int maxPlayerCount = m_keyManager.getPlayerLimit(room);

    // lock here because we get the player count, and then we potentially add the player to the list in the add packet method
    const std::lock_guard<std::mutex> playerCountLock(m_playerCountMutex);
    int currentConnectionCount = m_packetAccumulator.getConnectionCountWithoutConnection(identifier, room);

    if (!moonClient && currentConnectionCount >= maxPlayerCount) {
        close(std::move(handle), websocketpp::close::status::internal_endpoint_error, "Server is full. Please try again later.");
        return;
    }

    uint16_t length = message.size() - HASH_LENGTH;
    std::array<uint8_t, MAX_PACKET_SIZE> bytes {};
    std::memcpy(bytes.data(), message.data() + HASH_LENGTH, length);

    ReceivedPacket receivedPacket = {
            identifier,
            bytes,
            length,
    };
    m_packetAccumulator.addPacket(room, receivedPacket, std::move(handle));
}

uint16_t RelayServer::getIdentifier(connection_hdl* handlePointer) {
    const std::lock_guard<std::mutex> identifierLock(m_identifierMutex);

    if (!contains(m_handlePointerToIdentifierMap, handlePointer)) {
        return 0;
    }

    return m_handlePointerToIdentifierMap[handlePointer];
}

uint16_t RelayServer::claimIdentifier(connection_hdl* handlePointer) {
    const std::lock_guard<std::mutex> identifierLock(m_identifierMutex);

    uint16_t identifier = 0;
    for (int i = 1; i < UINT16_MAX; i++) {
        if (std::find(m_reservedIdentifiers.begin(), m_reservedIdentifiers.end(), i) != m_reservedIdentifiers.end()) {
            continue;
        }

        identifier = i;
    }

    // 0 is invalid identifier
    if (identifier == 0) {
        return 0;
    }

    m_reservedIdentifiers.emplace_back(identifier);
    m_handlePointerToIdentifierMap[handlePointer] = identifier;

    return identifier;
}

void RelayServer::releaseIdentifier(connection_hdl* handlePointer) {
    const std::lock_guard<std::mutex> identifierLock(m_identifierMutex);

    if (!contains(m_handlePointerToIdentifierMap, handlePointer)) {
        return;
    }

    uint16_t identifier = m_handlePointerToIdentifierMap[handlePointer];
    m_handlePointerToIdentifierMap.erase(handlePointer);

    m_reservedIdentifiers.erase(std::remove(m_reservedIdentifiers.begin(), m_reservedIdentifiers.end(), identifier), m_reservedIdentifiers.end());
}
