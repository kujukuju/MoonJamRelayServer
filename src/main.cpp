#include "Helpers.h"
#include "WebsocketServer.h"
#include "RelayServer.h"
#include "APIServer.h"

#include <thread>
#include <iostream>

int main() {
    const std::string secret = readFile("../secretkey.txt");

    std::thread apiServer = std::thread([&secret] {
        APIServer server(58006, secret);
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

    std::thread relayThread = std::thread([] {
        RelayServer server(58008);
        server.run();

        std::cerr << "Relay thread has exited on port 58008. This is bad. Restarting the server..." << std::endl;

        exit(1);
    });

    unsigned int cores = std::thread::hardware_concurrency();
    std::vector<std::thread> threads(cores);
    for (int i = 0; i < cores; i++) {
        threads.emplace_back([] {
        });
    }

    std::promise<void>().get_future().wait();

    return 0;
}
