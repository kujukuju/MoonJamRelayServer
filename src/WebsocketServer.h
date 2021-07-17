#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

class WebsocketServer {
public:
    explicit WebsocketServer(uint16_t port);

    void run();

    void onOpen(connection_hdl&& handle);

    void onClose(connection_hdl&& handle);

    void onMessage(connection_hdl&& handle, server::message_ptr&& message);

    void setOpenListener(std::function<void(connection_hdl&&)>&& listener);

    void setCloseListener(std::function<void(connection_hdl&&)>&& listener);

    void setMessageListener(std::function<void(connection_hdl&&, const std::string&)>&& listener);

private:
    server m_server;

    std::function<void(connection_hdl&&)> m_openListener;
    std::function<void(connection_hdl&&)> m_closeListener;
    std::function<void(connection_hdl&&, const std::string&)> m_messageListener;
};