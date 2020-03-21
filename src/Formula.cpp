#include "Formula.h"



Formula::Formula()
{
}

Formula::Formula(const Operator& op, const std::shared_ptr<Formula>& leftClause, const std::shared_ptr<Formula>& rightClause)
{
	m_operator = op;
	m_leftClause = leftClause;
	m_rightClause = rightClause;		
}


Formula::~Formula()
{
	m_leftClause.reset();
	m_rightClause.reset();
}
