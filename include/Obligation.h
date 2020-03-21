#ifndef OBLIGATION_H
#define OBLIGATION_H

#include "Literal.h"
#include <memory>
#include <set>
#include <spot/tl/formula.hh>
#include <stdio.h>
#include <vector>

class Obligation
{
  public:
    Obligation(const spot::formula& formula);
    virtual ~Obligation();
    virtual void Calculate() = 0;

  protected:
    spot::formula m_formula;
};

#endif
