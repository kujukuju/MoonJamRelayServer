#include "KeyManager.h"

#include <filesystem>
#include <iostream>

KeyManager::KeyManager() {
    refreshKeys();
}

void KeyManager::refreshKeys() {
    std::vector<AccessKeys> keys;

    for (const auto& entry : std::filesystem::directory_iterator("../keys")) {
        std::string filename = entry.path().string();
        std::string contents = readFile(filename);
        if (contents.length() != HASH_LENGTH * 2 + 1) {
            std::cerr << "Reading hash file with incorrect contents... " << filename << std::endl;
            continue;
        }

        char* contentCharacters = contents.data();
        std::array<char, HASH_LENGTH> moonKey {};
        std::array<char, HASH_LENGTH> playerKey {};
        for (int i = 0; i < HASH_LENGTH; i++) {
            moonKey[i] = contentCharacters[i];
            playerKey[i] = contentCharacters[i + HASH_LENGTH + 1];
        }

        keys.emplace_back(AccessKeys {
                moonKey,
                playerKey
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

bool KeyManager::hasKey(std::array<char, HASH_LENGTH> key) {
    const std::lock_guard<std::mutex> keyLock(m_keyMutex);

    for (auto& keys : m_keys) {
        if (keys.moonKey == key || keys.playerKey == key) {
            return true;
        }
    }

    return false;
};
