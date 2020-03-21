#ifndef OBLIGATIONSET_H
#define OBLIGATIONSET_H

#include "Obligation.h"
#include <iostream>
#include <set>
#include <spot/tl/parse.hh>
#include <spot/tl/simplify.hh>
#include <vector>

class ObligationSet : public Obligation
{
  public:
    ObligationSet(const spot::formula& formula);
    ~ObligationSet();
    virtual void Calculate();
    std::vector<std::set<spot::formula>> Get() const;

  private:
    bool IsLeftLiteral(spot::formula& olgLeft, spot::formula& olgRight);
    bool IsRightLiteral(spot::formula& olgLeft, spot::formula& olgRight);
    bool AreBothLiterals(spot::formula& olgLeft, spot::formula& olgRight);
    bool AreBothHaveChildren(spot::formula& olgLeft, spot::formula& olgRight);
    void DisplaySet(const std::vector<std::set<spot::formula>>& obligationsSet);

    std::pair<std::set<spot::formula>, std::vector<std::set<spot::formula>>>
        CalculateObligations(const spot::formula& olgFormula);

    std::vector<spot::formula> CalculateBasicCases(spot::formula& olgLeft, spot::formula& olgRight);
    spot::formula Olg(spot::formula formula);
    std::vector<std::set<spot::formula>> GetObligationsSet(const spot::formula& olgFormula);

    static spot::formula OrChildrenFormula(spot::formula formula, std::vector<std::set<spot::formula>>& olgSets);
    static bool IsMultipleOperators(const spot::formula& formula);

    std::vector<std::set<spot::formula>> m_obligations;
};

#endif