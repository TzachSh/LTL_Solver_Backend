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

    z3::solver solver { CreateAndClause(obligation, context) };

    return IsConsistent(solver);
}

bool OlgChecker::IsConsistent(z3::solver& solver) const
{
    switch (solver.check())
    {
    case z3::sat:
        return true;
    case z3::unsat:
    default:
        return false;
    }
}

z3::solver OlgChecker::CreateAndClause(const std::set<spot::formula>& obligation, z3::context& context)
{
    z3::solver solver { context };

    for (const auto& literal : obligation)
    {
        if (IsNegation(literal))
        {
            AddNegationLiteral(context, solver, literal);
        }
        else
        {
            AddLiteral(context, solver, literal);
        }
    }
    return solver;
}

void OlgChecker::AddLiteral(z3::context& context, z3::solver& solver, const spot::formula& literal)
{
    solver.add(context.bool_const(literal.ap_name().c_str()));
}

void OlgChecker::AddNegationLiteral(z3::context& context, z3::solver& solver, const spot::formula& literal)
{
    spot::formula child { literal.get_child_of(spot::op::Not) };
    solver.add(!context.bool_const(child.ap_name().c_str()));
}

bool OlgChecker::IsConsistent(crow::websocket::connection& conn)
{
    for (const auto& obligation : m_obligationSet.Get())
    {
        if (CheckObligation(obligation))
        {
            HandleFoundObligation(conn, obligation);
            return true;
        }
    }

    return false;
}

void OlgChecker::HandleFoundObligation(crow::websocket::connection& conn,
                                       const std::set<spot::formula>& obligation) const
{
    std::ostringstream olgStream;
    olgStream << "Found a consistent obligation: ";

    ObligationSet::ExtractOlgSetToStream(olgStream, obligation);

    conn.send_text(olgStream.str());
}