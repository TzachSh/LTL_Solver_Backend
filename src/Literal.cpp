#include "Literal.h"



Literal::Literal(const std::string& variable, const bool isNegation)
{
	m_variable = variable;
	m_isNegation = isNegation;
}


Literal::~Literal()
{
}
