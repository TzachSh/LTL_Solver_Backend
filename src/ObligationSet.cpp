#include "ObligationSet.h"

ObligationSet::ObligationSet(const spot::formula& formula) : Obligation(formula) {}

ObligationSet::~ObligationSet() = default;

void ObligationSet::Calculate()
{
    spot::formula olgFormula { Olg(m_formula) };
    m_obligations = GetObligationsSet(olgFormula);
}

bool ObligationSet::IsLeftLiteral(spot::formula& olgLeft, spot::formula& olgRight)
{
    return olgLeft.is_literal() && !olgRight.is_literal();
}

bool ObligationSet::IsRightLiteral(spot::formula& olgLeft, spot::formula& olgRight)
{
    return !olgLeft.is_literal() && olgRight.is_literal();
}

bool ObligationSet::AreBothLiterals(spot::formula& olgLeft, spot::formula& olgRight)
{
    return olgLeft.is_literal() && olgRight.is_literal();
}

bool ObligationSet::AreBothHaveChildren(spot::formula& olgLeft, spot::formula& olgRight)
{
    return !olgLeft.is_literal() && !olgRight.is_literal();
}

std::vector<spot::formula> ObligationSet::CalculateBasicCases(spot::formula& olgLeft, spot::formula& olgRight)
{
    std::vector<spot::formula> olgResult;

    if (IsLeftLiteral(olgLeft, olgRight))
    {
        for (const auto& child : olgRight)
        {
            olgResult.push_back(spot::formula::Or({ child, olgLeft }));
        }
    }
    else if (IsRightLiteral(olgLeft, olgRight))
    {
        for (const auto& child : olgLeft)
        {
            olgResult.push_back(spot::formula::Or({ child, olgRight }));
        }
    }
    else if (AreBothLiterals(olgLeft, olgRight))
    {
        olgResult.push_back(spot::formula::Or({ olgLeft, olgRight }));
    }
    else if (AreBothHaveChildren(olgLeft, olgRight))
    {
        for (const auto& childLeft : olgLeft)
        {
            for (const auto& childRight : olgResult)
            {
                olgResult.push_back(spot::formula::Or({ childLeft, childRight }));
            }
        }
    }

    return olgResult;
}

spot::formula ObligationSet::Olg(spot::formula formula)
{
    if (formula.is_tt() || formula.is_ff() || formula.is_literal())
    {
        return formula;
    }

    switch (formula.kind())
    {
    case spot::op::X:
        return Olg(formula[ static_cast<int>(Child::LEFT) ]);
    case spot::op::R:
    case spot::op::U:
        return Olg(formula[ static_cast<int>(Child::RIGHT) ]);
    case spot::op::Or:
    case spot::op::And:
    {
        spot::formula olgLeft { Olg(formula[ static_cast<int>(Child::LEFT) ]) };
        spot::formula olgRight { Olg(formula[ static_cast<int>(Child::RIGHT) ]) };

        std::vector<spot::formula> olgResult { spot::formula::And(CalculateBasicCases(olgLeft, olgRight)) };

        std::vector<spot::formula> finalResult;
        for (const auto& formulaChild : formula)
        {
            for (const auto& child : olgResult)
            {
                finalResult.push_back(spot::formula::Or({ child, Olg(formulaChild) }));
            }
        }

        std::size_t lastElementIndex = (finalResult.size() - 1);
        spot::formula lastRecursionFormula = finalResult[ lastElementIndex ];
        return (formula.kind() == spot::op::And) ? spot::formula::And({ lastRecursionFormula })
                                                 : spot::formula::Or({ lastRecursionFormula });
    }
    default:
        break;
    }

    return formula;
}

spot::formula ObligationSet::OrChildrenFormula(spot::formula formula, std::vector<std::set<spot::formula>>& olgSets)
{
    std::set<spot::formula> olgSet;

    for (const auto& child : formula)
    {
        if (IsMultipleOperators(formula)) // Fixed: children of Not won't be inserted, only the literal itself.
        {
            olgSet.insert(child);
        }
    }

    if (!olgSet.empty())
    {
        olgSets.push_back(olgSet);
    }

    return formula.map(OrChildrenFormula, olgSets);
}

bool ObligationSet::IsMultipleOperators(const spot::formula& formula)
{
    return formula.size() > 1;
}

std::pair<std::set<spot::formula>, std::vector<std::set<spot::formula>>>
    ObligationSet::CalculateObligations(const spot::formula& olgFormula)
{
    std::set<spot::formula> orSet;
    std::vector<std::set<spot::formula>> andSets;

    for (auto child : olgFormula)
    {
        switch (child.kind())
        {
        case spot::op::ap:
        case spot::op::Not:
            orSet.insert(child);
            break;
        case spot::op::And:
            child.map(OrChildrenFormula, andSets);
            break;
        default:
            break;
        }
    }

    return std::make_pair(orSet, andSets);
}

std::vector<std::set<spot::formula>> ObligationSet::GetObligationsSet(const spot::formula& olgFormula)
{
    std::pair<std::set<spot::formula>, std::vector<std::set<spot::formula>>> obligations =
        CalculateObligations(olgFormula);

    std::set<spot::formula> orSet { obligations.first };
    std::vector<std::set<spot::formula>> obligationsSet { obligations.second };

    std::vector<std::set<spot::formula>> result;
    for (const auto& literal : orSet)
    {
        for (auto obligation : obligationsSet)
        {
            obligation.insert(literal);
            auto formulaIter = std::find(result.begin(), result.end(), obligation);
            if (formulaIter == result.end())
            {
                result.push_back(std::set<spot::formula> { obligation });
            }
            else
            {
                result.erase(formulaIter);
            }
        }
    }

    return result;
}
std::vector<std::set<spot::formula>> ObligationSet::Get() const
{
    return m_obligations;
}

std::ostream& operator<<(std::ostream& out, const ObligationSet& obligationSet)
{
    out << "Obligations Set: { ";
    for (const auto& obligation : obligationSet.Get())
    {
        out << "{ ";
        for (const auto& literal : obligation)
        {
            out << literal << " ";
        }
        out << "} ";
    }
    out << "}" << std::endl;

    return out;
}
