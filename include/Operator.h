
#ifndef OPERATOR_H
#define OPERATOR_H

#include "Constants.h"
#include <cstdio>
#include <string>

class Operator
{
  public:
    Operator(const std::string& value, OpType type);
    Operator();
    ~Operator();

  private:
    std::string m_value;
    OpType m_type;
};

#endif