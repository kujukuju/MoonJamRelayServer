#pragma once

#include <string>

static const int HASH_LENGTH = 4;

std::string randomString(int length);

bool fileExists(const std::string& path);

std::string readFile(const std::string& path);

void writeFile(const std::string& name, const std::string& content);

bool deleteFile(const std::string& name);

std::array<char, HASH_LENGTH> convertHash(const std::string& hash);