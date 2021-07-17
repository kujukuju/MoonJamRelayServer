#pragma once

#include "Helpers.h"

#include <cpphttplib/httplib.h>

#include <cstdint>
#include <unordered_set>

struct AccessKeys {
    std::array<char, HASH_LENGTH> moonKey;
    std::array<char, HASH_LENGTH> playerKey;
};

class APIServer {
public:
    explicit APIServer(uint32_t port, const std::string& secret);

    void run();

    void refreshKeys();

private:
    bool hasKey(std::array<char, HASH_LENGTH> key);

    httplib::Server m_server;

    const uint32_t m_port;

    std::mutex m_keyMutex;
    std::vector<AccessKeys> m_keys;
};
