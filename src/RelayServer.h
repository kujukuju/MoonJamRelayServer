#pragma once

#include "WebsocketServer.h"

#include <cstdint>

class RelayServer {
public:
    explicit RelayServer(uint32_t port);

    void run();

private:
    void onOpen(connection_hdl&& handle);

    void onClose(connection_hdl&& handle);

    void onMessage(connection_hdl&& handle, const std::string& message);

    WebsocketServer m_server;
};
