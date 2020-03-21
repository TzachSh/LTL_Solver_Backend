#ifndef STATE_H
#define STATE_H


#include "Literal.h"
#include <set>
#include <vector>

typedef std::vector<std::set<Literal>> TransitionsDataType;

class State
{
public:
	State();
	State(const Formula& formula, const TransitionsDataType& m_transitions, const std::shared_ptr<State>& nextState);
	~State();

private:
	Formula m_formula;
	std::vector<std::set<Literal>> m_transitions;
	std::shared_ptr<State> m_nextState;
};

#endif