#pragma once

#include "Helpers.h"

#include <mutex>
#include <vector>

struct AccessKeys {
    std::array<char, HASH_LENGTH> moonKey;
    std::array<char, HASH_LENGTH> playerKey;
};

class KeyManager {
public:
    explicit KeyManager() = default;

    std::array<char, HASH_LENGTH> getRoom(std::array<char, HASH_LENGTH> key);

    bool hasKey(std::array<char, HASH_LENGTH> key);

    void refreshKeys();

private:
    std::mutex m_keyMutex;
    std::vector<AccessKeys> m_keys;
};
