#ifndef OBLIGATIONFORMULA_H
#define OBLIGATIONFORMULA_H

#include "Literal.h"
#include "Obligation.h"
#include "ObligationSet.h"
#include <memory>
#include <set>
#include <vector>

class ObligationFormula : public Obligation
{
  public:
    explicit ObligationFormula(const spot::formula& formula);
    ~ObligationFormula() override;
    void Calculate() override;
    spot::formula& Get();
    void Display();
    static spot::formula OF(spot::formula formula);
    //    static spot::formula TraverseOF(spot::formula f);

  private:
    spot::formula m_OF;
};

#endif
