#include "Automaton.h"



Automaton::Automaton(const State& initialState, const std::vector<State>& states)
{
	m_initialState = initialState;
	m_states = states;
}


Automaton::~Automaton()
{
}
