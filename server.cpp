#define _WIN32_WINNT 0x0A00

#include "httplib.h"
#include "blockchain.h"
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;
using namespace std;

int main() {
    Blockchain blockchain;
    httplib::Server svr;

    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },
        { "Access-Control-Allow-Methods", "GET, POST, OPTIONS" },
        { "Access-Control-Allow-Headers", "Content-Type" }
    });

    svr.Options("/.*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
    });

    // Health check
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"status":"ok"})", "application/json");
    });

    // Export blockchain
    svr.Get("/chain", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(blockchain.exportChainJSON(), "application/json");
    });

    // Register user
    svr.Post("/register", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            if (!body.contains("userID")) {
                res.status = 400;
                return;
            }

            bool ok = blockchain.registerUser(body["userID"]);

            json response;
            response["success"] = ok;

            if (!ok) res.status = 409;

            res.set_content(response.dump(), "application/json");
        }
        catch (...) {
            res.status = 400;
        }
    });

    // Create block
    svr.Post("/create", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            if (!body.contains("creator") || !body.contains("content")) {
                res.status = 400;
                return;
            }

            bool ok = blockchain.createBlock(
                body["creator"],
                body["content"]
            );

            json response;
            response["success"] = ok;

            if (!ok) res.status = 409;

            res.set_content(response.dump(), "application/json");
        }
        catch (...) {
            res.status = 400;
        }
    });

    // Verify ownership
    svr.Post("/verify", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            if (!body.contains("creator") || !body.contains("content")) {
                res.status = 400;
                return;
            }

            bool valid = blockchain.verifyOwnership(
                body["creator"],
                body["content"]
            );

            json response;
            response["valid"] = valid;

            res.set_content(response.dump(), "application/json");
        }
        catch (...) {
            res.status = 400;
        }
    });

    cout << "Server running at http://localhost:18080\n";
    svr.listen("0.0.0.0", 18080);
}
