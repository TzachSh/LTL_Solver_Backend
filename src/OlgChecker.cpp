#include "OlgChecker.h"

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
        return false;
    }
}

bool OlgChecker::IsConsistent(crow::websocket::connection& conn)
{
    for (const auto& obligation : m_obligationSet.Get())
    {
        if (CheckObligation(obligation))
        {
            std::ostringstream olgStream;
            olgStream << "Found a consistent obligation: ";
            ObligationSet::ExtractOlgSetToStream(olgStream, obligation);
            conn.send_text(olgStream.str());
            return true;
        }
    }

    return false;
}