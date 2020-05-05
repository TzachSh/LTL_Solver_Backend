#include "State.h"

State::State(spot::formula formula) : m_formula { std::move(formula) } {}

State::~State() = default;

void State::AddTransition(const std::pair<spot::formula, State>& transition)
{
    m_transitions.push_back(transition);
}

spot::formula State::GetFormula()
{
    return m_formula;
}

std::vector<std::pair<spot::formula, State>> State::GetTransitions()
{
    return m_transitions;
}