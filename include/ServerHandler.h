//
// Created by tzach on 19/04/2020.
//

#ifndef C___PROJECT_SERVERHANDLER_H
#define C___PROJECT_SERVERHANDLER_H

#include "crow_all.h"
#include <mutex>
#include <unordered_set>
#include <vector>

typedef struct
{
    std::string formula;
    bool isSat;
    long execTime;
} FormulaInfo;

class ServerHandler
{
  public:
    void HandleWSOnOpen(crow::websocket::connection& conn);
    void HandleWSOnClose(crow::websocket::connection& conn, const std::string& reason);
    void HandleWSOnMessage(crow::websocket::connection& conn, const std::string& data);
    crow::response HandleFetchExperimentsData();
    crow::response HandleResetExperiments();

  private:
    std::mutex m_mtx;
    std::unordered_set<crow::websocket::connection*> m_users;
    std::vector<FormulaInfo> m_formulasInfo;
};

#endif // C___PROJECT_SERVERHANDLER_H
