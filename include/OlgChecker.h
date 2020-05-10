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
    void AddNegationLiteral(z3::context& context, z3::solver& solver, const spot::formula& literal);
    void AddLiteral(z3::context& context, z3::solver& solver, const spot::formula& literal);
    z3::solver CreateAndClause(const std::set<spot::formula>& obligation, z3::context& context);
    bool IsConsistent(z3::solver& solver) const;
    void HandleFoundObligation(crow::websocket::connection& conn, const std::set<spot::formula>& obligation) const;
};

#endif