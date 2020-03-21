
#ifndef OPERATOR_H
#define OPERATOR_H

#include "Constants.h"
#include <stdio.h>
#include <string>

class Operator
{
public:
	Operator(const std::string& value, const OpType type);
	Operator();
	~Operator();

private:
	std::string m_value;
	OpType m_type;
};

#endif