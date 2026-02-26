#include "blockchain.h"
#include "crypto.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "json.hpp"
using json = nlohmann::json;

using namespace std;

// marks the starting block of the blockchain.
Blockchain::Blockchain() {
    Block genesis(0, 0, 0, "system", 0);
    chain.push_back(genesis);
}

// using the hash of the prev-block to calculate next.
unsigned long Blockchain::computeBlockHash(const Block &b) {
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

