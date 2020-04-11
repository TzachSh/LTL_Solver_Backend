#include "Automaton.h"

Automaton::Automaton(spot::formula formula) : m_initialFormula { std::move(formula) }
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
            if (!(NormalForm::IsEquals(state.GetFormula(), nextState)))
            {
                statesQueue.push(nextState);
            }

            state.AddTransition(transition);
        }

        m_states.push_back(state);
    }

    Display();
}

Automaton::~Automaton() = default;

spot::formula Automaton::GetFormula(const std::pair<spot::formula, spot::formula>& transition)
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

void Automaton::Display()
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