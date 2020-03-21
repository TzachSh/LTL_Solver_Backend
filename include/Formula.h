#ifndef FORMULA_H
#define FORMULA_H


#include "Operator.h"
#include <memory>

class Formula
{
public:
	Formula();
	Formula(const Operator& op, const std::shared_ptr<Formula>& leftClause, const std::shared_ptr<Formula>& rightClause);
	~Formula();

private:
	Operator m_operator;
	std::shared_ptr<Formula> m_leftClause;
	std::shared_ptr<Formula> m_rightClause;
};

#endif