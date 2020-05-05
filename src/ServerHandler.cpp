//
// Created by tzach on 19/04/2020.
//

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

    CROW_LOG_INFO << "websocket received data: " << data;

    FormulaInfo formulaInfo;
    formulaInfo.formula = data;

    auto start { std::chrono::steady_clock::now() };

    conn.send_text("SERVER RECEIVED: " + data);
    conn.send_text("************* STARTING ALGORITHM *************");

    Parser parser;
    spot::formula formula { parser.ParseFormula(data) };
    if (formula.is_tt())
    {
        conn.send_text(data + " is equivalent to true");
        conn.send_text(data + " is always satisfiable!");
        formulaInfo.isSat = true;
    }
    else if (formula.is_ff())
    {
        conn.send_text(data + " is equivalent to false");
        conn.send_text(data + " is unsatisfiable!");
        formulaInfo.isSat = false;
    }
    else
    {
        conn.send_text("Calculating Obligations Set of: " + data);
        ObligationSet obligationSet { formula };
        obligationSet.Calculate();
        conn.send_text(obligationSet.str());

        OlgChecker olgChecker { obligationSet };
        conn.send_text("Checking for a consistent Obligation...");

        if (olgChecker.IsConsistent(conn))
        {
            conn.send_text(data + " is Satisfiable!");
            formulaInfo.isSat = true;
        }
        else
        {
            conn.send_text("No consistent Obligation found => Satisfiability is still unknown!");

            TransitionsSystem transitionsSystem { formula };
            transitionsSystem.Build(conn);
            if (transitionsSystem.IsSatisfiable())
            {
                formulaInfo.isSat = true;
            }
            else
            {
                conn.send_text(data + " is not Satisfiable!");
                formulaInfo.isSat = false;
            }
        }
    }
    auto end { std::chrono::steady_clock::now() };
    long execTime { std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() };

    formulaInfo.execTime = execTime;
    m_formulasInfo.push_back(formulaInfo);

    conn.send_text("********************************************");
    conn.send_text("Calculation Time: " + std::to_string(execTime) + " ms");
    conn.send_text("********************************************");
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