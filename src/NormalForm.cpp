#include "NormalForm.h"

std::map<spot::formula, spot::formula> NormalForm::m_NFStore;

NormalForm::NormalForm(spot::formula formula) : m_formula { std::move(formula) } {}

NormalForm::~NormalForm() = default;

void NormalForm::Calculate(crow::websocket::connection& conn)
{
    std::vector<spot::formula> elements { m_formula };
    m_formula.map(GetElementsByOrder, elements);

    CalculateElementsNF(elements);

    if (IsEquals(m_formula, m_NFStore[ m_formula ]))
    {
        conn.send_text(spot::str_psl(m_NFStore[ m_formula ]));
        conn.send_text("NF Calculated Successfully !!");
    }
}

spot::formula NormalForm::NF(spot::formula& formula)
{
    if (formula.is_literal() || formula.is_tt() || formula.is_ff())
    {
        HandleLiteralInsertion(formula);
    }
    else
    {
        spot::formula resultNF { ApplyOperation(formula) };
        if (!IsNFStored(formula))
        {
            m_NFStore[ formula ] = resultNF;
        }
    }

    return m_NFStore[ formula ];
}

spot::formula NormalForm::ApplyOperation(spot::formula& formula)
{
    switch (formula.kind())
    {
    case spot::op::Or:
        return ApplyOr(formula);
    case spot::op::And:
        return ApplyAnd(formula);
    case spot::op::X:
        return ApplyX(formula);
    case spot::op::U:
        return ApplyU(formula);
    case spot::op::R:
        return ApplyR(formula);
    default:
        return formula;
    }
}

bool NormalForm::IsEquals(const spot::formula& formulaA, const spot::formula& formulaB)
{
    return spot::are_equivalent(formulaA, formulaB);
}

std::vector<spot::formula> NormalForm::GetNaryResult(std::stack<spot::formula>& result)
{
    std::vector<spot::formula> opResult;
    while (!result.empty())
    {
        spot::formula elementNF { m_NFStore[ result.top() ] };
        result.pop();
        opResult.push_back(elementNF);
    }

    return opResult;
}

void NormalForm::HandleLiteralInsertion(spot::formula& literal)
{
    if (IsNFStored(literal))
    {
        return;
    }

    spot::formula literalNF { spot::formula::And({ literal, spot::formula::X(spot::formula::tt()) }) };
    m_NFStore[ literal ] = literalNF;
}

spot::formula NormalForm::GetElementsByOrder(spot::formula formula, std::vector<spot::formula>& elements)
{
    elements.insert(elements.begin(), formula);
    return formula.map(GetElementsByOrder, elements);
}

spot::formula NormalForm::ApplyR(spot::formula& formula)
{
    spot::formula left { formula[ static_cast<int>(Child::LEFT) ] };
    spot::formula right { formula[ static_cast<int>(Child::RIGHT) ] };

    spot::formula leftFormula { CalculateRLeftFormula(m_NFStore[ left ], m_NFStore[ right ]) };
    spot::formula nextFormula { ApplyRAnd(m_NFStore[ right ], GetReleaseXFormula(left, right)) };

    return spot::formula::Or({ leftFormula, nextFormula });
}

spot::formula NormalForm::CalculateRLeftFormula(const spot::formula& leftNF, const spot::formula& rightNF)
{
    return spot::formula::And({ leftNF, rightNF });
}

spot::formula NormalForm::ApplyRAnd(spot::formula& rightNF, const spot::formula& next)
{
    if (rightNF.kind() != spot::op::Or)
    {
        return spot::formula::And({ rightNF, next });
    }

    std::vector<spot::formula> andResult;
    for (const auto& child : rightNF)
    {
        andResult.push_back(spot::formula::And({ child, next }));
    }
    return spot::formula::Or(andResult);
}

spot::formula NormalForm::GetReleaseXFormula(const spot::formula& leftFormula, const spot::formula& rightFormula)
{
    return spot::formula::X(spot::formula::R(leftFormula, rightFormula));
}

spot::formula NormalForm::ApplyU(spot::formula& formula)
{
    spot::formula left { formula[ static_cast<int>(Child::LEFT) ] };
    spot::formula right { formula[ static_cast<int>(Child::RIGHT) ] };

    spot::formula nextFormula { ApplyUAnd(m_NFStore[ left ], GetUntilXFormula(left, right)) };
    return spot::formula::Or({ m_NFStore[ right ], nextFormula });
}

spot::formula NormalForm::ApplyUAnd(spot::formula& leftNF, const spot::formula& next)
{
    if (leftNF.kind() != spot::op::Or)
    {
        return spot::formula::And({ leftNF, next });
    }

    std::vector<spot::formula> andResult;
    for (const auto& child : leftNF)
    {
        andResult.push_back(spot::formula::And({ child, next }));
    }
    return spot::formula::Or(andResult);
}

spot::formula NormalForm::GetUntilXFormula(const spot::formula& leftFormula, const spot::formula& rightFormula)
{
    return spot::formula::X(spot::formula::U(leftFormula, rightFormula));
}

spot::formula NormalForm::ApplyAnd(spot::formula& formula)
{
    std::vector<std::vector<spot::formula>> formulasSets { ConvertFormulaToSets(formula) };
    std::vector<spot::formula> andResult { PerformeAndOnSets(formulasSets) };
    spot::formula simplified { Parser::Simplify(spot::formula::Or(andResult)) };

    return simplified.is_ff() ? spot::formula::Or(andResult) : simplified;
}

std::vector<spot::formula> NormalForm::PerformeAndBetweenTwoSets(const std::vector<spot::formula>& setA,
                                                                 const std::vector<spot::formula>& setB)
{
    std::vector<spot::formula> result;

    for (const auto& formulaA : setA)
    {
        for (const auto& formulaB : setB)
        {
            result.push_back(spot::formula::And({ formulaA, formulaB }));
        }
    }

    return result;
}

std::vector<spot::formula> NormalForm::PerformeAndOnSets(std::vector<std::vector<spot::formula>>& andResultHelper)
{
    std::vector<spot::formula> andResultSet { PerformeAndBetweenTwoSets(
        andResultHelper[ static_cast<int>(Child::LEFT) ], andResultHelper[ static_cast<int>(Child::RIGHT) ]) };

    const int NEXT_CHILD_INDEX = 2;
    for (int i = NEXT_CHILD_INDEX; i < andResultHelper.size(); i++)
    {
        andResultSet = PerformeAndBetweenTwoSets(andResultSet, andResultHelper[ i ]);
    }

    return andResultSet;
}

std::vector<std::vector<spot::formula>> NormalForm::ConvertFormulaToSets(spot::formula& formula)
{
    std::vector<std::vector<spot::formula>> andResultHelper;

    for (const auto& child : formula)
    {
        std::vector<spot::formula> andSet;
        spot::formula NF { m_NFStore[ child ] };
        if (NF.kind() != spot::op::Or)
        {
            andSet.push_back(NF);
        }
        else
        {
            for (auto const& child : NF)
            {
                andSet.push_back(child);
            }
        }
        andResultHelper.push_back(andSet);
    }

    return andResultHelper;
}

spot::formula NormalForm::ApplyOr(spot::formula& formula)
{
    std::vector<spot::formula> orResult;

    for (const auto& child : formula)
    {
        orResult.push_back(m_NFStore[ child ]);
    }

    return spot::formula::Or(orResult);
}

spot::formula NormalForm::ApplyX(spot::formula& formula)
{
    std::vector<spot::formula> nextResult;

    for (const auto& child : formula)
    {
        nextResult.push_back(spot::formula::X(child));
    }

    return spot::formula::Or(nextResult);
}

bool NormalForm::IsNFStored(const spot::formula& formula)
{
    return m_NFStore.find(formula) != m_NFStore.end();
}

void NormalForm::DisplaySet(const std::pair<spot::formula, spot::formula>& set, crow::websocket::connection& conn) const
{
    conn.send_text(spot::str_psl(spot::formula::And({ set.first, set.second })));
}

std::set<std::pair<spot::formula, spot::formula>> NormalForm::ConvertToSet(crow::websocket::connection& conn)
{
    spot::formula NF { m_NFStore[ m_formula ] };

    if (NF.kind() != spot::op::Or)
    {
        if (NormalForm::IsEquals(spot::formula::ff(), NF))
        {
            return {};
        }

        std::pair<spot::formula, spot::formula> set { std::make_pair(NF, spot::formula::X(spot::formula::tt())) };
        DisplaySet(set, conn);
        return { set };
    }

    std::set<std::pair<spot::formula, spot::formula>> setsResult;
    for (auto child : m_NFStore[ m_formula ])
    {
        std::pair<std::set<spot::formula>, std::set<spot::formula>> set;
        child.traverse(TranslateNFToSet, set);
        const auto simplifiedSet { SimplifySet(set) };
        if (!IsEquals(simplifiedSet.first, spot::formula::ff()))
        {
            setsResult.insert(simplifiedSet);
            DisplaySet(SimplifySet(set), conn);
        }
    }

    return setsResult;
}

std::vector<spot::formula> NormalForm::SimplifyNextFormulas(const std::set<spot::formula>& nextFormulas)
{
    bool isSimplified { false };
    std::vector<spot::formula> simplifiedFormulas;

    for (const auto& formula : nextFormulas)
    {
        for (const auto& storedNF : m_NFStore)
        {
            if (IsEquals(storedNF.second, formula) && !storedNF.second.is_ff())
            {
                isSimplified = true;
                simplifiedFormulas.push_back(storedNF.first);
            }
        }
    }

    if (!isSimplified)
    {
        simplifiedFormulas.resize(nextFormulas.size());
        std::copy(nextFormulas.begin(), nextFormulas.end(), simplifiedFormulas.begin());
    }
    return simplifiedFormulas;
}

spot::formula NormalForm::ApplyAndNextFormulas(const std::vector<spot::formula>& nextFormulas)
{
    std::vector<spot::formula> andResult;

    for (const auto& formula : nextFormulas)
    {
        andResult.push_back(formula);
    }
    return spot::formula::And(andResult);
}

spot::formula NormalForm::ConstructSetAndFormula(const std::set<spot::formula>& literalsSet)
{
    std::stack<spot::formula> result;

    for (const auto& literal : literalsSet)
    {
        result.push(literal);
    }

    return spot::formula::And(GetNaryResult(result));
}

std::pair<spot::formula, spot::formula>
    NormalForm::SimplifySet(std::pair<std::set<spot::formula>, std::set<spot::formula>>& set)
{
    std::pair<spot::formula, spot::formula> result;

    result.first = ConstructSetAndFormula(set.first);
    result.second = SimplifyNexts(set.second);

    return result;
}

spot::formula NormalForm::SimplifyNexts(const std::set<spot::formula>& nextFormulas)
{
    std::vector<spot::formula> simplifiedFormulas { SimplifyNextFormulas(nextFormulas) };

    spot::formula andApplied { ApplyAndNextFormulas(simplifiedFormulas) };
    spot::formula andSimplified { Parser::Simplify(andApplied) };

    return andSimplified.is_ff() ? spot::formula::X(andApplied) : spot::formula::X(andSimplified);
}

bool NormalForm::TranslateNFToSet(spot::formula& NF,
                                  std::pair<std::set<spot::formula>, std::set<spot::formula>>& resultSet)
{
    if (NF.is_literal())
    {
        resultSet.first.insert(NF);
        return true;
    }
    else if (NF.kind() == spot::op::X)
    {
        resultSet.second.insert(NF[ static_cast<int>(Child::LEFT) ]);
        return true;
    }

    return false;
}

void NormalForm::CalculateElementsNF(const std::vector<spot::formula>& elements)
{
    for (auto element : elements)
    {
        m_NFStore[ element ] = NF(element);
    }
}