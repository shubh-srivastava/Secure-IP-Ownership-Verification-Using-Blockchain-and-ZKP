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

public:
    Blockchain();

    // User & blockchain operations
    bool registerUser(const string &userID);
    bool createBlock(const string &creatorID, const string &content);
    bool verifyOwnership(const string &creatorID, const string &content);

    // Console view (CLI)
    void viewChain();

    // REST API support
    string exportChainJSON() const;
};
#endif // BLOCKCHAIN_H
