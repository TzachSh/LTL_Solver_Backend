#include "OlgChecker.h"
#include <z3++.h>

OlgChecker::OlgChecker(const ObligationSet& obligationSet) : m_obligationSet { obligationSet } {};

OlgChecker::~OlgChecker() = default;

bool OlgChecker::IsNegation(const spot::formula& literal)
{
    return literal.kind() == spot::op::Not;
}

bool OlgChecker::CheckObligation(const std::set<spot::formula>& obligation)
{
    z3::context context;
    z3::solver solver { context };
    std::vector<z3::expr> andClause;

    for (const auto& literal : obligation)
    {
        if (IsNegation(literal))
        {
            spot::formula child { literal.get_child_of(spot::op::Not) };
            z3::expr variable { context.bool_const(child.ap_name().c_str()) };
            solver.add(!variable);
        }
        else
        {
            z3::expr variable { context.bool_const(literal.ap_name().c_str()) };
            solver.add(variable);
        }
    }

    switch (solver.check())
    {
    case z3::sat:
        return true;
    case z3::unsat:
    case z3::unknown:
        return false;
    }

    return false;
}

bool OlgChecker::IsConsistent()
{
    for (const auto& obligation : m_obligationSet.Get())
    {
        if (CheckObligation(obligation))
        {
            return true;
        }
    }

    return false;
}