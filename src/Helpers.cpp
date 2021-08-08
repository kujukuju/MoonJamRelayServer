#include "Helpers.h"

#include <random>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <filesystem>

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
	std::ifstream stream(path);
	return stream.good();
}

void readFile(const std::string& path, std::string& contents) {
	std::ifstream file(path);
	const uintmax_t fileSize = fs::file_size(path);
	contents.reserve(static_cast<size_t>(fileSize));
	contents.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	contents.erase(std::remove(contents.begin(), contents.end(), '\r'), contents.end());
	contents.erase(std::remove(contents.begin(), contents.end(), '\n'), contents.end());
	file.close();
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
	const char* characters = hash.data();
	for (int i = 0; i < HASH_LENGTH; i++) {
		value[i] = characters[i];
	}

	return value;
}

std::string print(std::array<char, HASH_LENGTH> hash) {
	std::string stringified;
	stringified.reserve(HASH_LENGTH);
	std::memcpy(stringified.data(), hash.data(), HASH_LENGTH);

	return stringified;
}
