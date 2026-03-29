#include "blockchain.h"
#include "crypto.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <stdexcept>
#include "json.hpp"

#ifdef USE_POSTGRESQL
#if __has_include(<libpq-fe.h>)
#include <libpq-fe.h>
#elif __has_include(<postgresql/libpq-fe.h>)
#include <postgresql/libpq-fe.h>
#else
#error "USE_POSTGRESQL is set but libpq headers were not found."
#endif
#endif

using json = nlohmann::json;

using namespace std;

namespace {
    const char *STATE_FILE = "blockchain_state.json";
    const char *POSTGRES_ENV_PRIMARY = "BLOCKCHAIN_POSTGRES_CONNINFO";
    const char *POSTGRES_ENV_FALLBACK = "DATABASE_URL";

#ifdef USE_POSTGRESQL
    bool execCommand(PGconn *conn, const string &sql, const string &context) {
        PGresult *result = PQexec(conn, sql.c_str());
        if (!result || PQresultStatus(result) != PGRES_COMMAND_OK) {
            cout << "PostgreSQL error while " << context << ": "
                 << PQerrorMessage(conn) << "\n";
            if (result) {
                PQclear(result);
            }
            return false;
        }

        PQclear(result);
        return true;
    }

    bool execParamsCommand(PGconn *conn,
                           const char *sql,
                           int paramCount,
                           const char *const *paramValues,
                           const string &context) {
        PGresult *result = PQexecParams(conn,
                                        sql,
                                        paramCount,
                                        nullptr,
                                        paramValues,
                                        nullptr,
                                        nullptr,
                                        0);
        if (!result || PQresultStatus(result) != PGRES_COMMAND_OK) {
            cout << "PostgreSQL error while " << context << ": "
                 << PQerrorMessage(conn) << "\n";
            if (result) {
                PQclear(result);
            }
            return false;
        }

        PQclear(result);
        return true;
    }
#endif
}

// marks the starting block of the blockchain.
Blockchain::Blockchain() : postgresEnabled(false) {
    configureStorage();

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

void Blockchain::configureStorage() {
#ifdef USE_POSTGRESQL
    postgresEnabled = true;

    const char *primary = getenv(POSTGRES_ENV_PRIMARY);
    const char *fallback = getenv(POSTGRES_ENV_FALLBACK);
    const char *connInfo = nullptr;
    const char *sourceEnv = nullptr;

    if (primary && *primary) {
        connInfo = primary;
        sourceEnv = POSTGRES_ENV_PRIMARY;
    } else if (fallback && *fallback) {
        connInfo = fallback;
        sourceEnv = POSTGRES_ENV_FALLBACK;
    }

    if (connInfo) {
        postgresConnInfo = connInfo;
        cout << "Storage backend: PostgreSQL (" << sourceEnv << ").\n";
    } else {
        postgresConnInfo.clear();
        cout << "Storage backend: PostgreSQL (libpq defaults; no "
             << POSTGRES_ENV_PRIMARY << " or "
             << POSTGRES_ENV_FALLBACK << " set).\n";
    }
#else
    postgresEnabled = false;
    postgresConnInfo.clear();
    cout << "Storage backend: JSON snapshot at " << STATE_FILE
         << " (compiled without USE_POSTGRESQL).\n";
#endif
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
    if (postgresEnabled) {
        return loadStateFromPostgres();
    }
    return loadStateFromJSON();
}

bool Blockchain::saveState() const {
    if (postgresEnabled) {
        return saveStateToPostgres();
    }
    return saveStateToJSON();
}

bool Blockchain::loadStateFromJSON() {
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

bool Blockchain::saveStateToJSON() const {
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

bool Blockchain::loadStateFromPostgres() {
#ifndef USE_POSTGRESQL
    cout << "PostgreSQL backend unavailable in this build.\n";
    return false;
#else
    PGconn *conn = PQconnectdb(postgresConnInfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        cout << "Unable to connect to PostgreSQL: "
             << PQerrorMessage(conn) << "\n";
        PQfinish(conn);
        return false;
    }

    const string schemaBlocks =
        "CREATE TABLE IF NOT EXISTS blocks ("
        "idx INTEGER PRIMARY KEY, "
        "prev_hash TEXT NOT NULL, "
        "ip_hash TEXT NOT NULL, "
        "block_hash TEXT NOT NULL, "
        "creator_id TEXT NOT NULL, "
        "ts BIGINT NOT NULL);";
    const string schemaUsers =
        "CREATE TABLE IF NOT EXISTS users ("
        "user_id TEXT PRIMARY KEY, "
        "public_key INTEGER NOT NULL);";

    if (!execCommand(conn, schemaBlocks, "creating blocks table") ||
        !execCommand(conn, schemaUsers, "creating users table")) {
        PQfinish(conn);
        return false;
    }

    PGresult *blockResult = PQexec(
        conn,
        "SELECT idx, prev_hash, ip_hash, block_hash, creator_id, ts "
        "FROM blocks ORDER BY idx ASC;");
    if (!blockResult || PQresultStatus(blockResult) != PGRES_TUPLES_OK) {
        cout << "PostgreSQL error while loading blocks: "
             << PQerrorMessage(conn) << "\n";
        if (blockResult) {
            PQclear(blockResult);
        }
        PQfinish(conn);
        return false;
    }

    vector<Block> loadedChain;
    unordered_map<string, User> loadedUsers;
    unordered_map<unsigned long, string> loadedRegistry;

    const int blockRows = PQntuples(blockResult);
    if (blockRows == 0) {
        PQclear(blockResult);
        PQfinish(conn);
        cout << "No persisted blockchain rows found in PostgreSQL.\n";
        return false;
    }

    try {
        for (int i = 0; i < blockRows; ++i) {
            int idx = stoi(PQgetvalue(blockResult, i, 0));
            unsigned long prevHash =
                static_cast<unsigned long>(stoull(PQgetvalue(blockResult, i, 1)));
            unsigned long ipHash =
                static_cast<unsigned long>(stoull(PQgetvalue(blockResult, i, 2)));
            unsigned long blockHash =
                static_cast<unsigned long>(stoull(PQgetvalue(blockResult, i, 3)));
            string creatorID = PQgetvalue(blockResult, i, 4);
            time_t timestamp =
                static_cast<time_t>(stoll(PQgetvalue(blockResult, i, 5)));

            Block block(idx, prevHash, ipHash, creatorID, blockHash);
            block.timestamp = timestamp;
            loadedChain.push_back(block);
        }
    } catch (const exception &) {
        cout << "PostgreSQL state contains invalid block values.\n";
        PQclear(blockResult);
        PQfinish(conn);
        return false;
    }

    PQclear(blockResult);

    for (size_t i = 0; i < loadedChain.size(); ++i) {
        const Block &block = loadedChain[i];

        if (block.index != static_cast<int>(i)) {
            cout << "Persisted chain has an invalid block index sequence.\n";
            PQfinish(conn);
            return false;
        }

        if (i == 0) {
            if (block.creatorID != "system" ||
                block.prevHash != 0 ||
                block.ipHash != 0 ||
                block.blockHash != 0) {
                cout << "Persisted genesis block is invalid.\n";
                PQfinish(conn);
                return false;
            }
            continue;
        }

        if (block.prevHash != loadedChain[i - 1].blockHash) {
            cout << "Persisted chain has a broken previous-hash link.\n";
            PQfinish(conn);
            return false;
        }

        if (computeBlockHash(block) != block.blockHash) {
            cout << "Persisted chain has a tampered block hash.\n";
            PQfinish(conn);
            return false;
        }

        if (loadedRegistry.count(block.ipHash)) {
            cout << "Persisted chain contains duplicate IP hashes.\n";
            PQfinish(conn);
            return false;
        }

        loadedRegistry[block.ipHash] = block.creatorID;
    }

    PGresult *userResult = PQexec(
        conn,
        "SELECT user_id, public_key FROM users;");
    if (!userResult || PQresultStatus(userResult) != PGRES_TUPLES_OK) {
        cout << "PostgreSQL error while loading users: "
             << PQerrorMessage(conn) << "\n";
        if (userResult) {
            PQclear(userResult);
        }
        PQfinish(conn);
        return false;
    }

    try {
        const int userRows = PQntuples(userResult);
        for (int i = 0; i < userRows; ++i) {
            string userID = PQgetvalue(userResult, i, 0);
            int publicKey = stoi(PQgetvalue(userResult, i, 1));
            loadedUsers.emplace(userID, User(userID, publicKey));
        }
    } catch (const exception &) {
        cout << "PostgreSQL state contains invalid user values.\n";
        PQclear(userResult);
        PQfinish(conn);
        return false;
    }

    PQclear(userResult);
    PQfinish(conn);

    chain = std::move(loadedChain);
    users = std::move(loadedUsers);
    rebuildIPRegistry();
    cout << "Loaded blockchain state from PostgreSQL"
         << " (blocks: " << chain.size()
         << ", users: " << users.size() << ").\n";
    return true;
#endif
}

bool Blockchain::saveStateToPostgres() const {
#ifndef USE_POSTGRESQL
    cout << "PostgreSQL backend unavailable in this build.\n";
    return false;
#else
    PGconn *conn = PQconnectdb(postgresConnInfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        cout << "Unable to connect to PostgreSQL: "
             << PQerrorMessage(conn) << "\n";
        PQfinish(conn);
        return false;
    }

    const string schemaBlocks =
        "CREATE TABLE IF NOT EXISTS blocks ("
        "idx INTEGER PRIMARY KEY, "
        "prev_hash TEXT NOT NULL, "
        "ip_hash TEXT NOT NULL, "
        "block_hash TEXT NOT NULL, "
        "creator_id TEXT NOT NULL, "
        "ts BIGINT NOT NULL);";
    const string schemaUsers =
        "CREATE TABLE IF NOT EXISTS users ("
        "user_id TEXT PRIMARY KEY, "
        "public_key INTEGER NOT NULL);";

    if (!execCommand(conn, schemaBlocks, "creating blocks table") ||
        !execCommand(conn, schemaUsers, "creating users table")) {
        PQfinish(conn);
        return false;
    }

    if (!execCommand(conn, "BEGIN;", "starting transaction")) {
        PQfinish(conn);
        return false;
    }

    bool ok = true;
    if (!execCommand(conn, "DELETE FROM blocks;", "clearing blocks") ||
        !execCommand(conn, "DELETE FROM users;", "clearing users")) {
        ok = false;
    }

    const char *insertBlockSQL =
        "INSERT INTO blocks (idx, prev_hash, ip_hash, block_hash, creator_id, ts) "
        "VALUES ($1, $2, $3, $4, $5, $6);";
    for (const auto &b : chain) {
        if (!ok) {
            break;
        }

        string idx = to_string(b.index);
        string prevHash = to_string(b.prevHash);
        string ipHash = to_string(b.ipHash);
        string blockHash = to_string(b.blockHash);
        string creatorID = b.creatorID;
        string timestamp = to_string(static_cast<long long>(b.timestamp));
        const char *params[] = {
            idx.c_str(),
            prevHash.c_str(),
            ipHash.c_str(),
            blockHash.c_str(),
            creatorID.c_str(),
            timestamp.c_str()
        };

        if (!execParamsCommand(conn,
                               insertBlockSQL,
                               6,
                               params,
                               "inserting block row")) {
            ok = false;
        }
    }

    const char *insertUserSQL =
        "INSERT INTO users (user_id, public_key) VALUES ($1, $2);";
    for (const auto &[userID, user] : users) {
        if (!ok) {
            break;
        }

        string publicKey = to_string(user.publicKey);
        const char *params[] = {
            userID.c_str(),
            publicKey.c_str()
        };

        if (!execParamsCommand(conn,
                               insertUserSQL,
                               2,
                               params,
                               "inserting user row")) {
            ok = false;
        }
    }

    if (ok) {
        ok = execCommand(conn, "COMMIT;", "committing transaction");
    } else {
        execCommand(conn, "ROLLBACK;", "rolling back transaction");
    }

    PQfinish(conn);
    return ok;
#endif
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

