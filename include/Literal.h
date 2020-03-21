#ifndef LITERAL_H
#define LITERAL_H


#include "Formula.h"
#include <string>

class Literal : public Formula
{
  public:
    Literal(const std::string& variable, bool isNegation);
    ~Literal();

  private:
    std::string m_variable;
    bool m_isNegation;
};

#endif