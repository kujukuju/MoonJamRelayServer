#include "Helpers.h"
#include "WebsocketServer.h"
#include "RelayServer.h"
#include "APIServer.h"
#include "KeyManager.h"
#include "PacketAccumulator.h"
#include "RoomManager.h"

#include <thread>
#include <iostream>

static_assert(sizeof(char) == 1, "Character size must be 1 byte.");

int main() {
    std::string contents;
    readFile("../secretkey.txt", contents);
    const std::string secret = contents;

    KeyManager keyManager;
    RoomManager roomManager;
    PacketAccumulator packetAccumulator(roomManager);

    std::thread apiServer = std::thread([&secret, &keyManager] {
        APIServer server(58006, secret, keyManager);
        server.run();

        std::cerr << "API thread has exited on port 58006. This is bad. Restarting the server..." << std::endl;

        exit(1);
    });

    // should setup another thread to read files out of the keys folder, so its async and simple, and so we can manually edit if we want

    std::thread pingThread = std::thread([] {
        WebsocketServer server(58007);
        server.setMessageListener([](connection_hdl&& handle, const std::string& message) {
            server::connection_type& connection = *(server::connection_type*) handle.lock().get();
            connection.send(nullptr, 0, websocketpp::frame::opcode::value::BINARY);
        });
        server.run();

        std::cerr << "Ping thread has exited on port 58007. This is bad. Restarting the server..." << std::endl;

        exit(1);
    });

    std::thread relayThread = std::thread([&keyManager, &packetAccumulator] {
        RelayServer server(58008, keyManager, packetAccumulator);
        server.run();

        std::cerr << "Relay thread has exited on port 58008. This is bad. Restarting the server..." << std::endl;

        exit(1);
    });

    std::promise<void>().get_future().wait();

    return 0;
}
