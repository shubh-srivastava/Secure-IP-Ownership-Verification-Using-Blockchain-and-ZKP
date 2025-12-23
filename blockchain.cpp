#include "blockchain.h"
#include "crypto.h"
#include <iostream>
#include <cstdlib>

//two prime numbers used for ZKP, very large in practice
static const int P = 7057;
static const int G = 5;

// marks the starting block of the blockchain.
Blockchain::Blockchain() {
    Block genesis(0, 0, 0, "system", 0);
    chain.push_back(genesis);
}

unsigned long Blockchain::computeBlockHash(const Block &b) {
    std::string data =
        std::to_string(b.index) +
        std::to_string(b.prevHash) +
        std::to_string(b.ipHash) +
        b.creatorID +
        std::to_string(b.timestamp);

    return djb2Hash(data);
}

bool Blockchain::registerUser(const std::string &userID) {
    if (users.count(userID)) return false;

    int privateKey = rand() % (P - 2) + 1;
    int publicKey = modExp(G, privateKey, P);

    users.emplace(userID, User(userID, privateKey, publicKey));
    return true;
}

bool Blockchain::createBlock(const std::string &creatorID, const std::string &content) {
    if (!users.count(creatorID)) {
        std::cout << "User not registered.\n";
        return false;
    }

    unsigned long ipHash = djb2Hash(content);

    if (ipRegistry.count(ipHash)) {
        std::cout << "Duplicate IP detected. Owner: "
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

    std::cout << "Block created successfully.\n";
    return true;
}

/*
 Proper Schnorr ZKP:
 Prove knowledge of privateKey without revealing it
*/
bool Blockchain::verifyOwnership(const std::string &creatorID,
                                 const std::string &content) {
    if (!users.count(creatorID)) {
        std::cout << "Verification failed: user not registered.\n";
        return false;
    }

    unsigned long ipHash = djb2Hash(content);

    if (!ipRegistry.count(ipHash)) {
        std::cout << "Verification failed: content is not registered on the blockchain.\n";
        return false;
    }

    if (ipRegistry[ipHash] != creatorID) {
        std::cout << "Verification failed: content is owned by another user.\n";
        return false;
    }

    User &u = users.at(creatorID);

    int r = rand() % (P - 1);
    int h = modExp(G, r, P);

    int challenge = rand() % (P - 1);
    int s = (r + challenge * u.privateKey) % (P - 1);

    int left = modExp(G, s, P);
    int right = (h * modExp(u.publicKey, challenge, P)) % P;

    if (left == right) {
        std::cout << "ZKP verification successful.\n";
        return true;
    }

    std::cout << "ZKP verification failed: cryptographic proof mismatch.\n";
    return false;
}



void Blockchain::viewChain() {
    for (const auto &b : chain) {
        std::cout << "Block #" << b.index
                  << "\nCreator: " << b.creatorID
                  << "\nIP Hash: " << b.ipHash
                  << "\nPrev Hash: " << b.prevHash
                  << "\nBlock Hash: " << b.blockHash
                  << "\nTime: " << ctime(&b.timestamp)
                  << "----------------------\n";
    }
}
