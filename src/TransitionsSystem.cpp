#include "TransitionsSystem.h"

TransitionsSystem::TransitionsSystem(const spot::formula& formula)
    : m_initialFormula { formula },
      m_isSatisfiable { false }
{
}

TransitionsSystem::~TransitionsSystem() = default;

void TransitionsSystem::Build(crow::websocket::connection& conn)
{
    conn.send_text("Constructing Transitions System...");

    std::queue<spot::formula> statesQueue;
    statesQueue.push(m_initialFormula);

    while (!statesQueue.empty())
    {
        State state { statesQueue.front() };
        statesQueue.pop();
        conn.send_text("Current State: " + spot::str_psl(state.GetFormula()));

        NormalForm NF { state.GetFormula() };
        conn.send_text("Calculating Normal Form...");
        NF.Calculate(conn);
        conn.send_text("NF Set:");
        const auto transitionsSet { NF.ConvertToSet(conn) };
        if (transitionsSet.size() == 0)
        {
            conn.send_text("NF Set is Empty");
        }

        for (auto const& transition : transitionsSet)
        {
            spot::formula nextState { GetFormula(transition) };
            conn.send_text("Next State: " + spot::str_psl(nextState));

            if (NormalForm::IsEquals(state.GetFormula(), nextState)) // Found an SCC
            {
                conn.send_text("Found an SCC");
                conn.send_text("Checking Weak Satisfaction...");

                spot::formula transitionCondition { transition.first };
                ObligationFormula of { state.GetFormula() };
                of.Calculate();

                WeakSatisfactionChecker wsChecker { transitionCondition, of.Get(), conn };
                if (wsChecker.Check())
                {
                    conn.send_text("The L(SCC) is a superset of the Obligation Formula's literals");
                    conn.send_text(spot::str_psl(m_initialFormula) + " is Satisfiable!");
                    m_isSatisfiable = true;
                    return;
                }
                else
                {
                    conn.send_text("The L(SCC) is not a superset of the Obligation Formula's literals");
                }
            }
            else
            {
                statesQueue.push(nextState);
            }

            state.AddTransition(transition);
        }
    }

    m_isSatisfiable = false;
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

bool TransitionsSystem::IsSatisfiable()
{
    return m_isSatisfiable;
}
