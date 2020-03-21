#ifndef PARSER_H
#define PARSER_H


#include "Constants.h"
#include <spot/tl/parse.hh>
#include <spot/tl/simplify.hh>
#include <spot/tl/print.hh>
#include <string>
#include <vector>
#include <stdio.h>


class Parser {
public:
  Parser();
  Parser(std::vector<std::string>& formulas);
  std::vector<spot::formula> Parse();
  ~Parser();

private:
  static spot::formula EliminateFG(spot::formula formula);
  spot::formula Simplify(spot::parsed_formula parsedFormula);
  std::vector<std::string> m_formulas;
};

#endif