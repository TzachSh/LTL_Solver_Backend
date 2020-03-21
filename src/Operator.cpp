#include "Operator.h"


Operator::Operator(const std::string& value, OpType type)
{
	m_value = value;
	m_type = type;
}

Operator::Operator()
{
}

Operator::~Operator()
{
}