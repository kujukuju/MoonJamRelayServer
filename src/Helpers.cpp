#include "Helpers.h"

#include <random>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

std::string randomString(int length) {
    static auto& characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{std::random_device{}()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(characters) - 2);

    std::string s;
    s.reserve(length);

    for (int i = 0; i < length; i++) {
        s += characters[pick(rg)];
    }

    return s;
}

bool fileExists(const std::string& path) {
    return fs::exists(path);
}

void readFile(std::string& content, const std::string& path) {
    std::ifstream stream(path);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    content = std::move(buffer.str());
    stream.close();
    content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
    content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
}

void writeFile(const std::string& name, const std::string& content) {
    std::ofstream outfile(name);
    outfile << content;
    outfile.close();
}

/*
    will only remove a file, if a intermediate directory is passed it will fail, could use
    remove_all and it will remove not only the intermediate directory but also it's sub files
    return types:
        remove		= boolean
        remove_all	= uintmax_t
*/
bool deleteFile(const std::string& name) {
    return fs::remove(name);
}

std::array<char, HASH_LENGTH> convertHash(const std::string& hash) {
    if (hash.size() != HASH_LENGTH) {
        std::cerr << "Invalid string when converting hash..." << std::endl;
        return {};
    }

    std::array<char, HASH_LENGTH> value {};
    for (int i = 0; i < HASH_LENGTH; i++) {
        value[i] = hash[i];
    }

    return value;
}

std::string print(std::array<char, HASH_LENGTH> hash) {
    std::string stringified;
    stringified.reserve(HASH_LENGTH);
    std::memcpy(stringified.data(), hash.data(), HASH_LENGTH);

    return stringified;
}

FileLogger logger;

LogMessage::~LogMessage() {
    m_ss << std::endl;
    logger.log(m_ss.str());
}
