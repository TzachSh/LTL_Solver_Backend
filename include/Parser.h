#ifndef PARSER_H
#define PARSER_H

#include "Constants.h"
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/simplify.hh>
#include <string>
#include <vector>

class Parser
{
  public:
    Parser();
    explicit Parser(std::vector<std::string>& formulas);
    static spot::formula Simplify(const spot::formula& formula);
    std::vector<spot::formula> Parse();
    ~Parser();

  private:
    static spot::formula EliminateFG(spot::formula formula);
    std::vector<std::string> m_formulas;
};

#endif