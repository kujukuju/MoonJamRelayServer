#pragma once

#include "WebsocketServer.h"
#include "Helpers.h"

#include <cstdint>

class KeyManager;
class PacketAccumulator;

class RelayServer {
public:
    explicit RelayServer(uint32_t port, KeyManager& keyManager, PacketAccumulator& packetAccumulator);

    void run();

    void close(connection_hdl&& handle, websocketpp::close::status::value status, const char* reason);

private:
    void onOpen(connection_hdl&& handle);

    void onClose(connection_hdl&& handle);

    void onMessage(connection_hdl&& handle, const std::string& message);

    uint16_t getIdentifier(connection_hdl* handlePointer);

    uint16_t claimIdentifier(connection_hdl* handlePointer);

    void releaseIdentifier(connection_hdl* handlePointer);

    WebsocketServer m_server;
    KeyManager& m_keyManager;
    PacketAccumulator& m_packetAccumulator;

    std::mutex m_identifierMutex;
    std::unordered_map<connection_hdl*, uint16_t> m_handlePointerToIdentifierMap;
    std::vector<uint16_t> m_reservedIdentifiers;
    std::unordered_map<uint16_t, std::array<char, HASH_LENGTH>> m_moonIdentifierToRoomMap;
};
