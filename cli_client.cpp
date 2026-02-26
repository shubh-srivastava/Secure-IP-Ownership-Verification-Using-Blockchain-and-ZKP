#define _WIN32_WINNT 0x0A00

#include "httplib.h"
#include "json.hpp"
#include "crypto.h"
#include <iostream>
#include <unordered_map>

using json = nlohmann::json;
using namespace std;

static const int P = 7057;
static const int G = 5;

struct Keys {
    int priv;
    int pub;
};

int main() {
    srand(time(nullptr));
    httplib::Client cli("http://localhost:18080");

    unordered_map<string, Keys> keys;

    while (true) {
        cout << "\n1. Health Check\n"
             << "2. Register User\n"
             << "3. Register IP\n"
             << "4. Verify Ownership (ZKP)\n"
             << "5. View Blockchain\n"
             << "6. Exit\n";

        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            auto res = cli.Get("/health");
            if (res && res->status == 200) {
                cout << res->body << "\n";
            } else {
                cout << "Server not reachable.\n";
            }
        } else if (choice == 2) {
            string userID;
            cout << "User ID: ";
            cin >> userID;

            int priv = rand() % (P - 2) + 1;
            int pub = modExp(G, priv, P);
            keys[userID] = {priv, pub};

            json body = {{"userID", userID}, {"publicKey", pub}};
            auto res = cli.Post("/register", body.dump(), "application/json");
            if (res) {
                cout << res->body << "\n";
            } else {
                cout << "Request failed.\n";
            }
        } else if (choice == 3) {
            string creator, content;
            cout << "Creator ID: ";
            cin >> creator;
            cin.ignore();
            cout << "Content: ";
            getline(cin, content);

            json body = {{"creator", creator}, {"content", content}};
            auto res = cli.Post("/create", body.dump(), "application/json");
            if (res) {
                cout << res->body << "\n";
            } else {
                cout << "Request failed.\n";
            }
        } else if (choice == 4) {
            string creator, content;
            cout << "Creator ID: ";
            cin >> creator;
            cin.ignore();
            cout << "Content: ";
            getline(cin, content);

            if (!keys.count(creator)) {
                cout << "No local key for this user. Register first.\n";
                continue;
            }

            unsigned long ipHash = djb2Hash(content);
            int r = rand() % (P - 1);
            int commitment = modExp(G, r, P);
            unsigned long h = djb2Hash(
                to_string(commitment) +
                to_string(keys[creator].pub) +
                to_string(ipHash) +
                creator
            );
            int challenge = static_cast<int>(h % (P - 1));
            int response = (r + challenge * keys[creator].priv) % (P - 1);

            json body = {
                {"creator", creator},
                {"content", content},
                {"commitment", commitment},
                {"response", response}
            };
            auto res = cli.Post("/verify", body.dump(), "application/json");
            if (res) {
                cout << res->body << "\n";
            } else {
                cout << "Request failed.\n";
            }
        } else if (choice == 5) {
            auto res = cli.Get("/chain");
            if (res) {
                cout << res->body << "\n";
            } else {
                cout << "Request failed.\n";
            }
        } else {
            break;
        }
    }

    return 0;
}
