#ifndef CONSISTENTOLGCHECKER_H
#define CONSISTENTOLGCHECKER_H

#include "ObligationSet.h"
#include <vector>

class OlgChecker
{
  public:
    OlgChecker(const ObligationSet& obligationSet);
    ~OlgChecker();
    bool IsConsistent();

  private:
    ObligationSet m_obligationSet;
    bool IsNegation(const spot::formula& literal);
    bool CheckObligation(std::set<spot::formula> obligation);
};

#endif