#include "KeyManager.h"

#include <filesystem>
#include <iostream>
#include <regex>

KeyManager::KeyManager() {
    refreshKeys();
}

void KeyManager::refreshKeys() {
    std::vector<AccessKeys> keys;
    std::string contents;
    for (const auto& entry : std::filesystem::directory_iterator("../keys")) {
        std::string filename = entry.path().string();
        readFile(contents, filename);
        if (contents.length() < HASH_LENGTH * 2 + 1) {
            std::cerr << "Reading hash file with incorrect contents... " << filename << std::endl;
            continue;
        }

        std::array<char, HASH_LENGTH> moonKey{};
        std::array<char, HASH_LENGTH> playerKey{};
        for (int i = 0; i < HASH_LENGTH; i++) {
            moonKey[i] = contents[i];
            playerKey[i] = contents[i + HASH_LENGTH + 1];
        }

        int playerLimit = DEFAULT_PLAYER_LIMIT;
        if (contents.length() > HASH_LENGTH * 2 + 1) {
            std::string playerLimitString = contents.substr(HASH_LENGTH * 2 + 2);
            std::regex validPlayerLimitRegex("[0-9]+");
            if (std::regex_match(playerLimitString, validPlayerLimitRegex)) {
                try {
                    playerLimit = std::stoi(playerLimitString);
                } catch (...) {
                    playerLimit = DEFAULT_PLAYER_LIMIT;
                }
            }
        }

        keys.emplace_back(AccessKeys {
                moonKey,
                playerKey,
                playerLimit
        });
    }

    const std::lock_guard<std::mutex> keyLock(m_keyMutex);
    m_keys.swap(keys);
}

std::array<char, HASH_LENGTH> KeyManager::getRoom(std::array<char, HASH_LENGTH> key) {
    const std::lock_guard<std::mutex> keyLock(m_keyMutex);

    for (auto& keys : m_keys) {
        if (keys.moonKey == key || keys.playerKey == key) {
            return keys.moonKey;
        }
    }

    return {};
}

int KeyManager::getPlayerLimit(std::array<char, HASH_LENGTH> key) {
    const std::lock_guard<std::mutex> keyLock(m_keyMutex);

    for (auto& keys : m_keys) {
        if (keys.moonKey == key || keys.playerKey == key) {
            return keys.playerLimit;
        }
    }

    return 0;
}

bool KeyManager::hasKey(std::array<char, HASH_LENGTH> key) {
    const std::lock_guard<std::mutex> keyLock(m_keyMutex);

    for (auto& keys : m_keys) {
        if (keys.moonKey == key || keys.playerKey == key) {
            return true;
        }
    }

    return false;
};
