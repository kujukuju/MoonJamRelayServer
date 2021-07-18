#include "APIServer.h"

#include "Helpers.h"
#include "KeyManager.h"

#include <filesystem>

APIServer::APIServer(uint32_t port, const std::string& secret, KeyManager& keyManager)
        : m_port(port),
          m_keyManager(keyManager) {
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
        m_keyManager.refreshKeys();

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
        } while (m_keyManager.hasKey(convertHash(playerHash)));

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
        } while (m_keyManager.hasKey(convertHash(moonHash)) || moonHash == playerHash);

        // write the files and fill the content with the corresponding hash
        writeFile("../keys/" + id + ".txt", moonHash + " " + playerHash);

        // refresh keys after the files are written to get them, not efficient but who cares
        m_keyManager.refreshKeys();

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
