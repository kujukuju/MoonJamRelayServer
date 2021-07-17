#include "APIServer.h"

#include "Helpers.h"

#include <filesystem>

APIServer::APIServer(uint32_t port, const std::string& secret)
        : m_port(port) {
    m_server.Post("/create", [this, &secret](const httplib::Request& request, httplib::Response& response) {
        if (!request.files.contains("key") || !request.files.contains("id")) {
            std::cerr << "Requested new keys without the proper data..." << std::endl;
            response.status = 511;
            return;
        }

        const std::string& key = request.files.find("key")->second.content;
        if (key != secret) {
            std::cerr << "Requested new keys without the proper key... " << std::endl;
            response.status = 511;
            return;
        }

        const std::string& id = request.files.find("id")->second.content;

        // refresh keys before it all starts to avoid duplicates
        refreshKeys();

        // create the new key
        int playerAttempts = 0;
        std::string playerHash;
        do {
            playerHash = randomString(HASH_LENGTH);

            // dont let it infinite loop
            playerAttempts++;
            if (playerAttempts > 1024) {
                response.status = 500;
                return;
            }
        } while (hasKey(convertHash(playerHash)));

        int moonAttempts = 0;
        std::string moonHash;
        do {
            moonHash = randomString(HASH_LENGTH);

            // dont let it infinite loop
            moonAttempts++;
            if (moonAttempts > 1024) {
                response.status = 500;
                return;
            }
        } while (hasKey(convertHash(moonHash)) || moonHash == playerHash);

        // write the files and fill the content with the corresponding hash
        writeFile("../keys/" + id + ".txt", moonHash + " " + playerHash);

        // refresh keys after the files are written to get them, not efficient but who cares
        refreshKeys();

        std::cout << "Created key pair for user " << id << "." << std::endl;

        response.status = 200;
        response.set_content("{\"moonkey\": \"" + moonHash + "\", \"playerkey\": \"" + playerHash + "\"}", "application/json");
    });

    m_server.Post("/delete", [&secret](const httplib::Request& request, httplib::Response& response) {
        if (!request.files.contains("key") || !request.files.contains("id")) {
            std::cerr << "Requested to delete keys without the proper data..." << std::endl;
            response.status = 511;
            return;
        }

        const std::string& key = request.files.find("key")->second.content;
        if (key != secret) {
            std::cerr << "Requested to delete keys without the proper key... " << std::endl;
            response.status = 511;
            return;
        }

        const std::string& id = request.files.find("id")->second.content;
        if (!fileExists("../keys/" + id + ".txt")) {
            response.status = 500;
            return;
        }

        // validate that the id is a simple letter number combo so they the server can't delete incorrect files
        std::regex validHashRegex("[0-9a-zA-Z]+");
        if (!std::regex_match(id, validHashRegex)) {
            response.status = 400;
            return;
        }

        if (!deleteFile("../keys/" + id + ".txt")) {
            response.status = 400;
            return;
        }

        std::cout << "Deleted key pair for user " << id << "." << std::endl;

        response.status = 200;
        response.set_content("{\"success\": true}", "application/json");
    });
}

void APIServer::run() {
    m_server.listen("0.0.0.0", m_port);
}

void APIServer::refreshKeys() {
    std::vector<AccessKeys> keys(m_keys.size());

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

bool APIServer::hasKey(std::array<char, HASH_LENGTH> key) {
    const std::lock_guard<std::mutex> keyLock(m_keyMutex);

    for (auto& keys : m_keys) {
        if (keys.moonKey == key) {
            return true;
        }

        if (keys.playerKey == key) {
            return true;
        }
    }

    return false;
};
