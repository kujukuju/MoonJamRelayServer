#pragma once

#include <string>
#include <array>

static const int HASH_LENGTH = 4;

namespace std {
    template <>
    struct hash<std::array<char, HASH_LENGTH>> {
        std::size_t operator()(const std::array<char, HASH_LENGTH>& room) const {
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

std::string readFile(const std::string& path);

void writeFile(const std::string& name, const std::string& content);

bool deleteFile(const std::string& name);

std::array<char, HASH_LENGTH> convertHash(const std::string& hash);
