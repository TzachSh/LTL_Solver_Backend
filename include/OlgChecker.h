#ifndef CONSISTENTOLGCHECKER_H
#define CONSISTENTOLGCHECKER_H

#include "ObligationSet.h"
#include "crow_all.h"
#include <vector>
#include <z3++.h>

class OlgChecker
{
  public:
    explicit OlgChecker(const ObligationSet& obligationSet);
    ~OlgChecker();
    bool IsConsistent(crow::websocket::connection& conn);

  private:
    ObligationSet m_obligationSet;
    bool IsNegation(const spot::formula& literal);
    bool CheckObligation(const std::set<spot::formula>& obligation);
};

#endif