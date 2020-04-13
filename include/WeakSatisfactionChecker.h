#ifndef WEAKSATISFACTIONCHECKER_H
#define WEAKSATISFACTIONCHECKER_H

#include "ObligationFormula.h"
#include "TransitionsSystem.h"

class WeakSatisfactionChecker
{
  public:
    WeakSatisfactionChecker(spot::formula& condition, const spot::formula& of);
    ~WeakSatisfactionChecker();
    bool Check();

  private:
    static bool StoreConditionLiterals(spot::formula formula, std::vector<spot::formula>& literalsStore);
    void StoreElementsSatisfactionInfo(const std::vector<spot::formula>& elements);
    void SaveInfo(spot::formula& formula);
    spot::formula IsExists(const std::vector<spot::formula>& conditionLiterals, const spot::formula& literal);
    spot::formula ApplyOperation(const spot::formula& formula);
    spot::formula ApplyOr(const spot::formula& formula);
    spot::formula ApplyAnd(const spot::formula& formula);
    std::vector<spot::formula> GetFormulaLiteralsInfo(const spot::formula& formula);

    std::map<spot::formula, spot::formula> m_satisfactionInfo;
    std::vector<spot::formula> m_conditionLiterals;
    spot::formula m_of;
};

#endif