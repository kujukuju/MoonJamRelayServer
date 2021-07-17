#include "WebsocketServer.h"
#include "RelayServer.h"

#include <cpphttplib/httplib.h>

#include <thread>
#include <iostream>

int main() {
    std::thread apiServer = std::thread([] {
        httplib::Server server;
        server.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
            res.set_content("Hello World!", "text/plain");
        });
        server.listen("0.0.0.0", 58006);

        std::cerr << "API thread has exited on port 58006. This is bad. Restarting the server..." << std::endl;

        exit(1);
    });

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
