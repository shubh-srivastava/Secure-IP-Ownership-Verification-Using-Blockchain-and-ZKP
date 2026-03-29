#include "crypto.h"
using namespace std;

// can use a better hashing protocol, but will be very computaionally high to run local
// this is proof of concept project, therefore djb2Hash used.
unsigned long djb2Hash(const string &input) {
    unsigned long hash = 5381;
    for (char c : input) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

int modExp(int base, int exp, int mod) {
    long long result = 1;
    long long b = base % mod;
    while (exp > 0) {
        if (exp & 1)
            result = (result * b) % mod;
        b = (b * b) % mod;
        exp >>= 1;
    }
    return (int)result;
}
