#pragma once

#include "Helpers.h"

#include <mutex>
#include <vector>

struct AccessKeys {
    std::array<char, HASH_LENGTH> moonKey;
    std::array<char, HASH_LENGTH> playerKey;
    int playerLimit = DEFAULT_PLAYER_LIMIT;
};

class KeyManager {
public:
    explicit KeyManager();

    std::array<char, HASH_LENGTH> getRoom(std::array<char, HASH_LENGTH> key);

    int getPlayerLimit(std::array<char, HASH_LENGTH> key);

    bool hasKey(std::array<char, HASH_LENGTH> key);

    void refreshKeys();

private:
    std::mutex m_keyMutex;
    std::vector<AccessKeys> m_keys;
};
