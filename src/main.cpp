#include "Constants.h"
#include "ServerHandler.h"
#include "crow_all.h"

int main(int argc, char** argv)
{
    crow::SimpleApp app;
    ServerHandler serverHandler;

    CROW_ROUTE(app, "/ws")
        .websocket()
        .onopen([&](crow::websocket::connection& conn) {
          serverHandler.HandleWSOnOpen(conn);
        })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
          serverHandler.HandleWSOnClose(conn, reason);
        })
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
            serverHandler.HandleWSOnMessage(conn, data);
        });

    CROW_ROUTE(app, "/experiments").methods("OPTIONS"_method, "GET"_method)([&]() {
        return serverHandler.HandleFetchExperimentsData();
    });

    CROW_ROUTE(app, "/reset").methods("OPTIONS"_method, "GET"_method)([&] {
        return serverHandler.HandleResetExperiments();
    });

    app.port(PORT).multithreaded().run();
}