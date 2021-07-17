#include "RelayServer.h"

RelayServer::RelayServer(const uint32_t port)
        : m_server(port) {
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

void RelayServer::onOpen(connection_hdl&& handle) {

}

void RelayServer::onClose(connection_hdl&& handle) {

}

void RelayServer::onMessage(connection_hdl&& handle, const std::string& message) {

}
