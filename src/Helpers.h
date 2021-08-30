#pragma once

#include <string>
#include <array>
#include <unordered_map>
#include <map>

static const int HASH_LENGTH = 4;

namespace std {
    template<>
    struct hash<std::array<char, HASH_LENGTH>> {
        std::size_t operator()(const std::array<char, HASH_LENGTH> &room) const {
            std::size_t value = 0;
            for (int i = 0; i < HASH_LENGTH; i++) {
                int mod = (int) (i % sizeof(std::size_t));
                int shifted = room[i] << (mod * 8);

                value ^= shifted;
            }

            return value;
        }
    };
}

std::string randomString(int length);

bool fileExists(const std::string& path);

void readFile(std::string& content, const std::string& path);

void writeFile(const std::string& name, const std::string& content);

bool deleteFile(const std::string& name);

std::array<char, HASH_LENGTH> convertHash(const std::string& hash);

std::string print(std::array<char, HASH_LENGTH>& hash);

template<class T, class V>
bool contains(const std::unordered_map<T, V>& map, const T& key) {
    return map.find(key) != map.end();
}

template<class T, class V>
bool contains(const std::map<T, V>& map, const T& key) {
    return map.find(key) != map.end();
}

template<class T, class V, class Z>
bool contains(const std::multimap<T, V>& map, const Z& key) {
    return map.find(key) != map.end();
}
