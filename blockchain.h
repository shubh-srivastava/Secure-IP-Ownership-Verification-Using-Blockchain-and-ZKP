#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <unordered_map>
#include <string>

#include "block.h"
#include "user.h"

using namespace std;

class Blockchain {
private:
    vector<Block> chain;
    unordered_map<unsigned long, string> ipRegistry;
    unordered_map<string, User> users;

    unsigned long computeBlockHash(const Block &b);
    int computeChallenge(int commitment, int publicKey,
                         unsigned long ipHash,
                         const string &creatorID) const;

public:
    static constexpr int P = 7057;
    static constexpr int G = 5;

    Blockchain();

    // User & blockchain operations
    bool registerUser(const string &userID, int publicKey);
    bool createBlock(const string &creatorID, const string &content);
    bool verifyOwnership(const string &creatorID,
                         const string &content,
                         int commitment,
                         int response);

    // Console view (CLI)
    void viewChain();

    // REST API support
    string exportChainJSON() const;
};
#endif // BLOCKCHAIN_H
