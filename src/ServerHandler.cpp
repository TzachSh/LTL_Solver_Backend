#include "ServerHandler.h"

void ServerHandler::HandleWSOnOpen(crow::websocket::connection& conn)
{
    CROW_LOG_INFO << "new websocket connection";
    std::lock_guard<std::mutex> _(m_mtx);
    m_users.insert(&conn);
}

void ServerHandler::HandleWSOnClose(crow::websocket::connection& conn, const std::string& reason)
{
    CROW_LOG_INFO << "websocket connection closed: " << reason;
    std::lock_guard<std::mutex> _(m_mtx);
    m_users.erase(&conn);
}

void ServerHandler::HandleWSOnMessage(crow::websocket::connection& conn, const std::string& data)
{
    std::lock_guard<std::mutex> _(m_mtx);

    SendInitMessages(conn, data);
    FormulaInfo formulaInfo { data };

    auto startTime { std::chrono::steady_clock::now() };

    conn.send_text("Parsing Formula...");
    spot::formula formula { ParseFormula(data) };
    conn.send_text("Parsed, Simplified and FG Eliminated: " + spot::str_psl(formula));

    RunAlgorithm(data, formula, conn, formulaInfo);

    auto endTime { std::chrono::steady_clock::now() };
    long execTime { std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() };

    SaveCalculationInfo(formulaInfo, execTime);
    SendCalculationTime(conn, execTime);
}

void ServerHandler::RunAlgorithm(const std::string& data, const spot::formula& formula,
                                 crow::websocket::connection& conn, FormulaInfo& formulaInfo) const
{
    if (formula.is_tt())
    {
        HandleTT(conn, data, formulaInfo);
    }
    else if (formula.is_ff())
    {
        HandleFF(conn, data, formulaInfo);
    }
    else
    {
        conn.send_text("Checking Satisfiability...");
        HandleSATCheck(data, formula, conn, formulaInfo);
    }
}

void ServerHandler::SaveCalculationInfo(FormulaInfo& formulaInfo, long execTime)
{
    formulaInfo.execTime = execTime;
    m_formulasInfo.push_back(formulaInfo);
}
void ServerHandler::SendCalculationTime(crow::websocket::connection& conn, long execTime) const
{
    conn.send_text("********************************************");
    conn.send_text("Calculation Time: " + std::to_string(execTime) + " ms");
    conn.send_text("********************************************");
}
void ServerHandler::SendInitMessages(crow::websocket::connection& conn, const std::string& data) const
{
    CROW_LOG_INFO << "websocket received data: " << data;
    conn.send_text("SERVER RECEIVED: " + data);
    conn.send_text("************* STARTING ALGORITHM *************");
}

spot::formula ServerHandler::ParseFormula(const std::string& data) const
{
    Parser parser;
    return parser.Parse(data);
}

void ServerHandler::HandleSATCheck(const std::string& data, const spot::formula& formula,
                                   crow::websocket::connection& conn, FormulaInfo& formulaInfo) const
{
    conn.send_text("Calculating Obligations Set of: " + data);
    ObligationSet obligationSet { CalculateOlgSet(conn, formula) };

    conn.send_text("Checking for a consistent Obligation...");
    OlgChecker olgChecker { obligationSet };

    if (olgChecker.IsConsistent(conn))
    {
        HandleConsistenOlg(data, conn, formulaInfo);
    }
    else
    {
        HandleInconsistentOlg(data, formula, conn, formulaInfo);
    }
}
void ServerHandler::HandleInconsistentOlg(const std::string& data, const spot::formula& formula,
                                          crow::websocket::connection& conn, FormulaInfo& formulaInfo) const
{
    conn.send_text("No consistent Obligation found => Satisfiability is still unknown!");

    conn.send_text("Constructing Transitions System...");
    TransitionsSystem transitionsSystem { formula };
    formulaInfo.isSat = transitionsSystem.Build(conn);

    if (!formulaInfo.isSat)
    {
        SendUnSATMsg(data, conn);
    }
}
void ServerHandler::SendUnSATMsg(const std::string& data, crow::websocket::connection& conn) const
{
    conn.send_text(data + " is not Satisfiable!");
}

void ServerHandler::HandleConsistenOlg(const std::string& data, crow::websocket::connection& conn,
                                       FormulaInfo& formulaInfo) const
{
    conn.send_text(data + " is Satisfiable!");
    formulaInfo.isSat = true;
}

ObligationSet ServerHandler::CalculateOlgSet(crow::websocket::connection& conn, const spot::formula& formula) const
{
    ObligationSet obligationSet { formula };

    obligationSet.Calculate();
    conn.send_text(obligationSet.str());

    return obligationSet;
}

void ServerHandler::HandleFF(crow::websocket::connection& conn, const std::string& data, FormulaInfo& formulaInfo) const
{
    conn.send_text(data + " is equivalent to false");
    conn.send_text(data + " is unsatisfiable!");
    formulaInfo.isSat = false;
}

void ServerHandler::HandleTT(crow::websocket::connection& conn, const std::string& data, FormulaInfo& formulaInfo) const
{
    conn.send_text(data + " is equivalent to true");
    conn.send_text(data + " is always satisfiable!");
    formulaInfo.isSat = true;
}

crow::response ServerHandler::HandleFetchExperimentsData()
{
    std::lock_guard<std::mutex> _(m_mtx);

    CROW_LOG_INFO << "experiments requested";
    crow::json::wvalue json;

    for (int i = 0; i < m_formulasInfo.size(); i++)
    {
        json[ i ][ "formula" ] = m_formulasInfo[ i ].formula;
        json[ i ][ "execTime" ] = m_formulasInfo[ i ].execTime;
        json[ i ][ "isSat" ] = m_formulasInfo[ i ].isSat;
    }
    CROW_LOG_INFO << "experiments completed";
    return crow::json::dump(json);
}

crow::response ServerHandler::HandleResetExperiments()
{
    std::lock_guard<std::mutex> _(m_mtx);
    CROW_LOG_INFO << "reset requested";

    m_formulasInfo.clear();
    CROW_LOG_INFO << "reset completed";
    return crow::response(SUCCESS_CODE);
}