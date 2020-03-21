#ifndef OBLIGATIONFORMULA_H
#define OBLIGATIONFORMULA_H

#include "Literal.h"
#include "Obligation.h"
#include <memory>
#include <set>
#include <vector>

class ObligationFormula : public Obligation
{
  public:
    explicit ObligationFormula(const spot::formula& formula);
    ~ObligationFormula() override;
    void Calculate() override;
};

#endif
