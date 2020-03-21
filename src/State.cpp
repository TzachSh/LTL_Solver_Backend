#include "State.h"

State::State()
{
}

State::State(const Formula& formula, const TransitionsDataType& transitions, const std::shared_ptr<State>& nextState)
{
	m_formula = formula;
	m_transitions = transitions;
	m_nextState = nextState;
}


State::~State()
{
}
