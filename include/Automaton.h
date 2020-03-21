#ifndef AUTOMATON_H
#define AUTOMATON_H


#include "State.h"
#include <vector>

class Automaton
{
public:
	Automaton(const State& initialState, const std::vector<State>& states);
	~Automaton();

private: 
	State m_initialState;
	std::vector<State> m_states;
};

#endif
