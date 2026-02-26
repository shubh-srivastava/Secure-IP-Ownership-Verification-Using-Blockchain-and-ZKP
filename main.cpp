#include <iostream>
#include <unordered_map>
#include "blockchain.h"
#include "crypto.h"

using namespace std;

int main() {
    srand(time(nullptr));

    Blockchain bc;
    unordered_map<string, int> privKeys;
    unordered_map<string, int> pubKeys;

    int choice;
     string user, content;

    while (true) {
        cout << "\n1. Register User\n"
                  << "2. Create Block\n"
                  << "3. Verify Ownership\n"
                  << "4. View Blockchain\n"
                  << "5. Exit\n";

        cin >> choice;
         cin.ignore();

        if (choice == 1) {
            cout << "User ID: ";
            cin >> user;
            int privateKey = rand() % (Blockchain::P - 2) + 1;
            int publicKey = modExp(Blockchain::G, privateKey, Blockchain::P);

            privKeys[user] = privateKey;
            pubKeys[user] = publicKey;

            if(bc.registerUser(user, publicKey)){
                cout << "User registered.\n";
            }
            else{
                cout << "User already exists.\n";
            }
        }
        else if(choice == 2){
            cout << "User ID: ";
            cin >> user;
            cin.ignore();
            cout << "Content: ";
            getline( cin, content);
            bc.createBlock(user, content);
        }
        else if (choice == 3){
            cout << "User ID: ";
            cin >> user;
            cin.ignore();
            cout << "Content: ";
            getline( cin, content);
            if (!privKeys.count(user) || !pubKeys.count(user)) {
                cout << "Client-side key not found for user.\n";
                continue;
            }

            unsigned long ipHash = djb2Hash(content);
            int r = rand() % (Blockchain::P - 1);
            int commitment = modExp(Blockchain::G, r, Blockchain::P);
            unsigned long h = djb2Hash(
                to_string(commitment) +
                to_string(pubKeys[user]) +
                to_string(ipHash) +
                user
            );
            int challenge = static_cast<int>(h % (Blockchain::P - 1));
            int response = (r + challenge * privKeys[user]) % (Blockchain::P - 1);

            bc.verifyOwnership(user, content, commitment, response);
        }
        else if (choice == 4){
            bc.viewChain();
        }
        else{
            break;
        }
    }

    return 0;
}
