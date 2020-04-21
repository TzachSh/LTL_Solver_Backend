#include "TransitionsSystem.h"
#include <include/WeakSatisfactionChecker.h>

TransitionsSystem::TransitionsSystem(const spot::formula& formula)
    : m_initialFormula { formula },
      m_isSatisfiable { false }
{
}

TransitionsSystem::~TransitionsSystem() = default;

void TransitionsSystem::Build(crow::websocket::connection& conn)
{
    std::queue<spot::formula> statesQueue;
    statesQueue.push(m_initialFormula);

    while (!statesQueue.empty())
    {
        State state { statesQueue.front() };
        statesQueue.pop();

        NormalForm NF { state.GetFormula() };
        NF.Calculate();
        NF.Display();

        for (auto const& transition : NF.ConvertNFToSet())
        {
            spot::formula nextState { GetFormula(transition) };
            if (NormalForm::IsEquals(state.GetFormula(), nextState)) // Found an SCC
            {
                spot::formula transitionCondition { transition.first };
                ObligationFormula of { state.GetFormula() };
                of.Calculate();

                WeakSatisfactionChecker wsChecker { transitionCondition, of.Get(), conn };
                if (wsChecker.Check())
                {
                    m_isSatisfiable = true;
                }
            }
            else
            {
                statesQueue.push(nextState);
            }

            state.AddTransition(transition);
        }

        m_states.push_back(state);
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

void TransitionsSystem::Display()
{
    std::cout << std::endl;
    std::cout << "States: " << m_states.size() << std::endl;
    for (auto state : m_states)
    {
        std::cout << "Formula: " << state.GetFormula() << std::endl;
        std::cout << "Transitions:" << std::endl;
        for (auto transition : state.GetTransitions())
        {
            std::cout << "Condition: " << transition.first << std::endl;
            std::cout << "Next State: " << transition.second.GetFormula() << std::endl;
        }
    }
}