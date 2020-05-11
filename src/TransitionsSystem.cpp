#include "TransitionsSystem.h"

TransitionsSystem::TransitionsSystem(const spot::formula& formula) : m_initialFormula { formula } {}
TransitionsSystem::~TransitionsSystem() = default;

bool TransitionsSystem::Build(crow::websocket::connection& conn)
{
    std::queue<spot::formula> statesQueue { InitTransitionsSystem() };

    while (!statesQueue.empty())
    {
        State state { GetCurrentState(statesQueue, conn) };

        auto transitionsSet { CalculateTransitions(state, conn) };
        for (auto transition : transitionsSet)
        {
            spot::formula nextState { GetNextState(conn, transition) };

            if (IsSCC(state, nextState))
            {
                if (CheckWeakSatisfaction(state, transition.first, conn))
                {
                    return true;
                }
            }
            else
            {
                InsertState(statesQueue, nextState);
            }

            state.AddTransition(transition);
        }
    }
    return false;
}

void TransitionsSystem::InsertState(std::queue<spot::formula>& statesQueue, const spot::formula& nextState)
{
    if (m_statesTrack.find(nextState) == m_statesTrack.end())
    {
        statesQueue.push(nextState);
        m_statesTrack.insert(nextState);
    }
}

std::queue<spot::formula> TransitionsSystem::InitTransitionsSystem()
{
    std::queue<spot::formula> statesQueue;
    statesQueue.push(m_initialFormula);

    return statesQueue;
}

bool TransitionsSystem::IsSCC(State& state, const spot::formula& nextState) const
{
    return Utilities::AreEquals(state.GetFormula(), nextState);
}

bool TransitionsSystem::CheckWeakSatisfaction(State& state, spot::formula& transitionCondition,
                                              crow::websocket::connection& conn)
{
    SendSCCInfoMsg(conn);

    spot::formula of { GetObligationFormula(state) };
    WeakSatisfactionChecker wsChecker { transitionCondition, of, conn };

    if (wsChecker.IsSatisfies())
    {
        return HandleSAT(conn);
    }
    else
    {
        return ContinueExploring(conn);
    }
}

bool TransitionsSystem::ContinueExploring(crow::websocket::connection& conn) const
{
    conn.send_text("The L(SCC) is not a superset of the Obligation Formula's literals");
    return false;
}

bool TransitionsSystem::HandleSAT(crow::websocket::connection& conn)
{
    conn.send_text("The L(SCC) is a superset of the Obligation Formula's literals");
    conn.send_text(spot::str_psl(m_initialFormula) + " is Satisfiable!");
    return true;
}

spot::formula TransitionsSystem::GetObligationFormula(State& state) const
{
    ObligationFormula of { state.GetFormula() };
    of.Calculate();
    return of.Get();
}

void TransitionsSystem::SendSCCInfoMsg(crow::websocket::connection& conn) const
{
    conn.send_text("Found an SCC");
    conn.send_text("Checking Weak Satisfaction...");
}

spot::formula TransitionsSystem::GetNextState(crow::websocket::connection& conn,
                                              const std::pair<spot::formula, spot::formula>& transition)
{
    spot::formula nextState { GetFormula(transition) };
    // conn.send_text("Next Formula State: " + spot::str_psl(nextState));
    return nextState;
}

std::set<std::pair<spot::formula, spot::formula>>
    TransitionsSystem::CalculateTransitions(State& state, crow::websocket::connection& conn)
{
    conn.send_text("Calculating Normal Form...");

    NormalForm NF { state.GetFormula() };
    NF.Calculate(conn);

    conn.send_text("NF Set:");
    auto transitionsSet { NF.ConvertToSet(conn) };

    if (transitionsSet.empty())
    {
        conn.send_text("NF Set is Empty");
    }

    return transitionsSet;
}

State TransitionsSystem::GetCurrentState(std::queue<spot::formula>& statesQueue, crow::websocket::connection& conn)
{
    State state { statesQueue.front() };
    statesQueue.pop();

    conn.send_text("Current Formula State: " + spot::str_psl(state.GetFormula()));
    return state;
}

spot::formula TransitionsSystem::GetFormula(const std::pair<spot::formula, spot::formula>& transition)
{
    spot::formula transitionFormula { transition.second };
    if (transitionFormula.is_tt() || transitionFormula.is_ff())
    {
        return transitionFormula;
    }
    else
    {
        return transitionFormula[ static_cast<int>(Child::LEFT) ];
    }
}