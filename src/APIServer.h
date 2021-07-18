#pragma once

#include "Helpers.h"

#include <cpphttplib/httplib.h>

#include <cstdint>
#include <unordered_set>

class KeyManager;
struct AccessKeys;

class APIServer {
public:
    explicit APIServer(uint32_t port, const std::string& secret, KeyManager& keyManager);

    void run();

private:
    httplib::Server m_server;
    KeyManager& m_keyManager;

    const uint32_t m_port;
};
