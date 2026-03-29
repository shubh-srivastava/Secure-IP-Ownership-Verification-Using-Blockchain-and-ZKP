#include "blockchain.h"
#include "crypto.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

using namespace std;

namespace {
    const char *STATE_FILE = "blockchain_state.json";
}

// marks the starting block of the blockchain.
Blockchain::Blockchain() {
    if (!loadState()) {
        initializeGenesis();
        cout << "Initialized fresh blockchain with genesis block.\n";
    }
}

void Blockchain::initializeGenesis() {
    chain.clear();
    ipRegistry.clear();
    users.clear();

    Block genesis(0, 0, 0, "system", 0);
    chain.push_back(genesis);
}

void Blockchain::rebuildIPRegistry() {
    ipRegistry.clear();

    for (size_t i = 1; i < chain.size(); ++i) {
        ipRegistry[chain[i].ipHash] = chain[i].creatorID;
    }
}

// using the hash of the prev-block to calculate next.
unsigned long Blockchain::computeBlockHash(const Block &b) const {
    string data =
        to_string(b.index) +
        to_string(b.prevHash) +
        to_string(b.ipHash) +
        b.creatorID +
        to_string(b.timestamp);

    return djb2Hash(data);
}

int Blockchain::computeChallenge(int commitment, int publicKey,
                                 unsigned long ipHash,
                                 const string &creatorID) const {
    string data =
        to_string(commitment) +
        to_string(publicKey) +
        to_string(ipHash) +
        creatorID;
    unsigned long h = djb2Hash(data);
    return static_cast<int>(h % (P - 1));
}

bool Blockchain::registerUser(const string &userID, int publicKey) {
    if (users.count(userID)) return false;

    users.emplace(userID, User(userID, publicKey));
    if (!saveState()) {
        users.erase(userID);
        cout << "Failed to persist user registry.\n";
        return false;
    }

    cout << "Persisted user registry after registering '" << userID
         << "' (users: " << users.size()
         << ", blocks: " << chain.size() << ").\n";

    return true;
}

bool Blockchain::createBlock(const string &creatorID, const string &content) {
    if (!users.count(creatorID)) {
        cout << "User not registered.\n";
        return false;
    }

    unsigned long ipHash = djb2Hash(content);

    if (ipRegistry.count(ipHash)) {
        cout << "Duplicate IP detected. Owner: "
             << ipRegistry[ipHash] << "\n";
        return false;
    }

    int idx = chain.size();
    unsigned long prevHash = chain.back().blockHash;

    Block temp(idx, prevHash, ipHash, creatorID, 0);
    unsigned long blockHash = computeBlockHash(temp);
    temp.blockHash = blockHash;

    chain.push_back(temp);
    ipRegistry[ipHash] = creatorID;
    if (!saveState()) {
        chain.pop_back();
        ipRegistry.erase(ipHash);
        cout << "Failed to persist blockchain state.\n";
        return false;
    }

    cout << "Persisted blockchain state after adding block #" << temp.index
         << " (blocks: " << chain.size()
         << ", users: " << users.size() << ").\n";
    cout << "Block created successfully.\n";
    return true;
}

/*
 Proper Schnorr ZKP:
 Prove knowledge of privateKey without revealing it
*/
bool Blockchain::verifyOwnership(const string &creatorID,
                                 const string &content,
                                 int commitment,
                                 int response) {
    if (!users.count(creatorID)) {
        cout << "Verification failed: user not registered.\n";
        return false;
    }

    unsigned long ipHash = djb2Hash(content);

    if (!ipRegistry.count(ipHash)) {
        cout << "Verification failed: content is not registered on the blockchain.\n";
        return false;
    }

    if (ipRegistry[ipHash] != creatorID) {
        cout << "Verification failed: content is owned by another user.\n";
        return false;
    }

    const User &u = users.at(creatorID);

    int challenge = computeChallenge(commitment, u.publicKey, ipHash, creatorID);
    int left = modExp(G, response, P);
    int right = (commitment * modExp(u.publicKey, challenge, P)) % P;

    if (left == right) {
        cout << "ZKP verification successful.\n";
        return true;
    }

    cout << "ZKP verification failed: cryptographic proof mismatch.\n";
    return false;
}

void Blockchain::viewChain() {
    for (const auto &b : chain) {
        cout << "Block #" << b.index
             << "\nCreator: " << b.creatorID
             << "\nIP Hash: " << b.ipHash
             << "\nPrev Hash: " << b.prevHash
             << "\nBlock Hash: " << b.blockHash
             << "\nTime: " << ctime(&b.timestamp)
             << "----------------------\n";
    }
}

std::string Blockchain::exportChainJSON() const {
    json arr = json::array();

    for (const auto &b : chain) {
        arr.push_back({
            {"index", b.index},
            {"creator", b.creatorID},
            {"ipHash", b.ipHash},
            {"prevHash", b.prevHash},
            {"blockHash", b.blockHash},
            {"timestamp", b.timestamp}
        });
    }

    return arr.dump(2); // pretty-printed JSON
}

bool Blockchain::loadState() {
    ifstream in(STATE_FILE);
    if (!in.is_open()) {
        cout << "No persisted blockchain state found at " << STATE_FILE
             << ".\n";
        return false;
    }

    try {
        json state;
        in >> state;

        json chainJSON = state;
        json usersJSON = json::array();

        if (state.is_object()) {
            if (!state.contains("chain") || !state["chain"].is_array()) {
                cout << "Persisted state file is missing a valid chain array.\n";
                return false;
            }
            chainJSON = state["chain"];
            if (state.contains("users") && state["users"].is_array()) {
                usersJSON = state["users"];
            }
        } else if (!state.is_array()) {
            cout << "Persisted state file has an unsupported format.\n";
            return false;
        }

        vector<Block> loadedChain;
        unordered_map<string, User> loadedUsers;
        unordered_map<unsigned long, string> loadedRegistry;

        for (const auto &entry : chainJSON) {
            if (!entry.contains("index") ||
                !entry.contains("creator") ||
                !entry.contains("ipHash") ||
                !entry.contains("prevHash") ||
                !entry.contains("blockHash") ||
                !entry.contains("timestamp")) {
                cout << "Persisted chain contains an incomplete block entry.\n";
                return false;
            }

            Block block(
                entry["index"].get<int>(),
                entry["prevHash"].get<unsigned long>(),
                entry["ipHash"].get<unsigned long>(),
                entry["creator"].get<string>(),
                entry["blockHash"].get<unsigned long>()
            );
            block.timestamp = entry["timestamp"].get<time_t>();
            loadedChain.push_back(block);
        }

        if (loadedChain.empty()) {
            cout << "Persisted chain is empty.\n";
            return false;
        }

        for (size_t i = 0; i < loadedChain.size(); ++i) {
            const Block &block = loadedChain[i];

            if (block.index != static_cast<int>(i)) {
                cout << "Persisted chain has an invalid block index sequence.\n";
                return false;
            }

            if (i == 0) {
                if (block.creatorID != "system" ||
                    block.prevHash != 0 ||
                    block.ipHash != 0 ||
                    block.blockHash != 0) {
                    cout << "Persisted genesis block is invalid.\n";
                    return false;
                }
                continue;
            }

            if (block.prevHash != loadedChain[i - 1].blockHash) {
                cout << "Persisted chain has a broken previous-hash link.\n";
                return false;
            }

            if (computeBlockHash(block) != block.blockHash) {
                cout << "Persisted chain has a tampered block hash.\n";
                return false;
            }

            if (loadedRegistry.count(block.ipHash)) {
                cout << "Persisted chain contains duplicate IP hashes.\n";
                return false;
            }

            loadedRegistry[block.ipHash] = block.creatorID;
        }

        for (const auto &entry : usersJSON) {
            if (!entry.contains("userID") || !entry.contains("publicKey")) {
                cout << "Persisted user registry contains an invalid entry.\n";
                return false;
            }

            string userID = entry["userID"].get<string>();
            int publicKey = entry["publicKey"].get<int>();
            loadedUsers.emplace(userID, User(userID, publicKey));
        }

        chain = std::move(loadedChain);
        users = std::move(loadedUsers);
        rebuildIPRegistry();
        cout << "Loaded blockchain state from " << STATE_FILE
             << " (blocks: " << chain.size()
             << ", users: " << users.size() << ").\n";
        return true;
    }
    catch (...) {
        cout << "Failed to parse persisted blockchain state from "
             << STATE_FILE << ".\n";
        return false;
    }
}

bool Blockchain::saveState() const {
    json state;
    state["chain"] = json::array();
    state["users"] = json::array();

    for (const auto &b : chain) {
        state["chain"].push_back({
            {"index", b.index},
            {"creator", b.creatorID},
            {"ipHash", b.ipHash},
            {"prevHash", b.prevHash},
            {"blockHash", b.blockHash},
            {"timestamp", b.timestamp}
        });
    }

    for (const auto &[userID, user] : users) {
        state["users"].push_back({
            {"userID", userID},
            {"publicKey", user.publicKey}
        });
    }

    ofstream out(STATE_FILE);
    if (!out.is_open()) {
        return false;
    }

    out << state.dump(2);
    return out.good();
}

bool Blockchain::resetState() {
    vector<Block> oldChain = chain;
    unordered_map<unsigned long, string> oldRegistry = ipRegistry;
    unordered_map<string, User> oldUsers = users;

    initializeGenesis();

    if (!saveState()) {
        chain = std::move(oldChain);
        ipRegistry = std::move(oldRegistry);
        users = std::move(oldUsers);
        cout << "Failed to persist blockchain reset.\n";
        return false;
    }

    cout << "Blockchain reset complete. Persisted genesis block only.\n";
    return true;
}

