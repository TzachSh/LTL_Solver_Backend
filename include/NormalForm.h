#ifndef NORMALFORM_H
#define NORMALFORM_H

#include "Literal.h"
#include <iostream>
#include <memory>
#include <set>
#include <spot/tl/formula.hh>
#include <vector>

typedef std::vector<std::pair<std::set<spot::formula>, Formula>> nfDataType;

class NormalForm
{
  public:
    explicit NormalForm(const spot::formula& formula);
    ~NormalForm();
    void Calculate();

  private:
    static spot::formula CalculateDF(spot::formula formula, std::set<spot::formula> disjunctsSet);

    spot::formula m_formula;
    nfDataType m_store;
};

#endif