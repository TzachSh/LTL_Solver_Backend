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
    explicit FormulaInfo(const std::string& formula, const int depth = 0, bool isSat = false, long execTime = 0,
                         const bool isConsistent = false)
        : formula { formula },
          depth { depth },
          isSat { isSat },
          execTime { execTime },
          isConsistent { isConsistent }
    {
    }

    std::string formula;
    int depth;
    bool isSat;
    long execTime;
    bool isConsistent;
};

class ServerHandler
{
  public:
    ServerHandler();
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
    void InitInfoFields();

    std::mutex m_mtx;
    std::unordered_set<crow::websocket::connection*> m_users;
    std::vector<FormulaInfo> m_formulasInfo;
    std::map<InfoFields, std::string> m_infoFields;
    void SaveCalculationData(crow::websocket::connection& conn, FormulaInfo& formulaInfo, long execTime);
    spot::formula PerformParsing(crow::websocket::connection& conn, const std::string& data) const;
    int CalculateDepth(crow::websocket::connection& conn, spot::formula& formula) const;
};

#endif // C___PROJECT_SERVERHANDLER_H