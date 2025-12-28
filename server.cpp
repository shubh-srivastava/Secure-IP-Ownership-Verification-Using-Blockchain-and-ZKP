#define _WIN32_WINNT 0x0A00
#include "httplib.h"
#include "blockchain.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    Blockchain blockchain;
    httplib::Server svr;

    // Health check
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    // Register user
    svr.Post("/register", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        bool ok = blockchain.registerUser(body["user_id"]);
        res.set_content(
            ok ? "{\"status\":\"registered\"}" : "{\"error\":\"user_exists\"}",
            "application/json"
        );
    });

    // Create block
    svr.Post("/create", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        bool ok = blockchain.createBlock(body["user_id"], body["content"]);
        res.set_content(
            ok ? "{\"status\":\"block_created\"}" : "{\"error\":\"failed\"}",
            "application/json"
        );
    });

    // Verify ownership
    svr.Post("/verify", [&](const httplib::Request& req, httplib::Response& res) {
        auto body = json::parse(req.body);
        bool verified = blockchain.verifyOwnership(body["user_id"], body["content"]);
        res.set_content(
            json({{"verified", verified}}).dump(),
            "application/json"
        );
    });

    // View chain
    svr.Get("/chain", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(blockchain.exportChainJSON(), "application/json");
    });

    svr.listen("0.0.0.0", 18080);
}
