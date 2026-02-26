#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

struct User {
    string id;
    int publicKey;

    User(const string &uid, int pub)
        : id(uid), publicKey(pub) {}
};

#endif
