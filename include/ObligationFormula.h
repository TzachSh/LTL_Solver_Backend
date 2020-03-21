#ifndef OBLIGATIONFORMULA_H
#define OBLIGATIONFORMULA_H

#include "Literal.h"
#include "Obligation.h"
#include <memory>
#include <set>
#include <stdio.h>
#include <vector>

class ObligationFormula : public Obligation
{
  public:
    ObligationFormula(const spot::formula& formula);
    ~ObligationFormula();
    virtual void Calculate();
};

#endif
