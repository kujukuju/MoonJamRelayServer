#include "WebsocketServer.h"

WebsocketServer::WebsocketServer(const uint16_t port) {
    // initialize asio transport
    m_server.init_asio();

    // turn off logging
    // TODO does this turn it off or on? lol
    m_server.clear_access_channels(websocketpp::log::alevel::all);

    // register handler callbacks
    m_server.set_open_handler([this](auto&& PH1) {
        onOpen(std::forward<decltype(PH1)>(PH1));
    });
    m_server.set_close_handler([this](auto&& PH1) {
        onClose(std::forward<decltype(PH1)>(PH1));
    });
    m_server.set_message_handler([this](auto&& PH1, auto&& PH2) {
        onMessage(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });

    // listen on specified port
    m_server.listen(port);

    // start the server accept loop
    m_server.start_accept();
}

void WebsocketServer::run() {
    // start the asio io_service run loop
    try {
        std::cout << "Websocket running..." << std::endl;
        m_server.run();
        std::cout << "Websocket server closed." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Websocket run exception: " << e.what() << std::endl;
    }
}

void WebsocketServer::onOpen(connection_hdl&& handle) {
    std::cout << "Raw socket open: " << &handle << std::endl;
    if (!m_openListener) {
        return;
    }

    m_openListener(std::move(handle));
}

void WebsocketServer::onClose(connection_hdl&& handle) {
    if (!m_closeListener) {
        return;
    }

    m_closeListener(std::move(handle));
}

void WebsocketServer::onMessage(connection_hdl&& handle, server::message_ptr&& msg) {
    if (!m_messageListener) {
        return;
    }

    const std::string& payload = msg->get_payload();
    m_messageListener(std::move(handle), payload);
}

void WebsocketServer::setOpenListener(std::function<void(connection_hdl&&)>&& listener) {
    m_openListener = std::move(listener);
}

void WebsocketServer::setCloseListener(std::function<void(connection_hdl&&)>&& listener) {
    m_closeListener = std::move(listener);
}

void WebsocketServer::setMessageListener(std::function<void(connection_hdl&&, const std::string&)>&& listener) {
    m_messageListener = std::move(listener);
}
