#ifndef C___PROJECT_SERVERHANDLER_H
#define C___PROJECT_SERVERHANDLER_H

#include "ObligationSet.h"
#include "OlgChecker.h"
#include "Parser.h"
#include "TransitionsSystem.h"
#include "crow_all.h"
#include <mutex>
#include <spot/tl/formula.hh>
#include <unordered_set>
#include <vector>

struct FormulaInfo
{
    explicit FormulaInfo(const std::string& formula, bool isSat = false, long execTime = 0)
        : formula { formula },
          isSat { isSat },
          execTime { execTime }
    {
    }

    std::string formula;
    bool isSat;
    long execTime;
};

class ServerHandler
{
  public:
    void HandleWSOnOpen(crow::websocket::connection& conn);
    void HandleWSOnClose(crow::websocket::connection& conn, const std::string& reason);
    void HandleWSOnMessage(crow::websocket::connection& conn, const std::string& data);
    crow::response HandleFetchExperimentsData();
    crow::response HandleResetExperiments();

  private:
    void HandleTT(crow::websocket::connection& conn, const std::string& data, FormulaInfo& formulaInfo) const;
    void HandleFF(crow::websocket::connection& conn, const std::string& data, FormulaInfo& formulaInfo) const;
    ObligationSet CalculateOlgSet(crow::websocket::connection& conn, const spot::formula& formula) const;
    void HandleSATCheck(const std::string& data, const spot::formula& formula, crow::websocket::connection& conn,
                        FormulaInfo& formulaInfo) const;
    spot::formula ParseFormula(const std::string& data) const;
    void SendInitMessages(crow::websocket::connection& conn, const std::string& data) const;
    void SendCalculationTime(crow::websocket::connection& conn, long execTime) const;
    void SaveCalculationInfo(FormulaInfo& formulaInfo, long execTime);

    void RunAlgorithm(const std::string& data, const spot::formula& formula, crow::websocket::connection& conn,
                      FormulaInfo& formulaInfo) const;
    void HandleConsistenOlg(const std::string& data, crow::websocket::connection& conn, FormulaInfo& formulaInfo) const;
    void HandleInconsistentOlg(const std::string& data, const spot::formula& formula, crow::websocket::connection& conn,
                               FormulaInfo& formulaInfo) const;
    void SendUnSATMsg(const std::string& data, crow::websocket::connection& conn) const;

    std::mutex m_mtx;
    std::unordered_set<crow::websocket::connection*> m_users;
    std::vector<FormulaInfo> m_formulasInfo;
};

#endif // C___PROJECT_SERVERHANDLER_H