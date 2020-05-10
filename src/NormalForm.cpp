#include "NormalForm.h"
std::map<spot::formula, spot::formula> NormalForm::m_NFStore;

NormalForm::NormalForm(spot::formula formula) : m_formula { std::move(formula) } {}
NormalForm::~NormalForm() = default;

void NormalForm::Calculate(crow::websocket::connection& conn)
{
    std::vector<spot::formula> elements { m_formula };
    m_formula.map(GetElementsByOrder, elements);

    CalculateElementsNF(elements);

    if (AreEquals(m_formula, m_NFStore[ m_formula ]))
    {
        conn.send_text(spot::str_psl(m_NFStore[ m_formula ]));
        conn.send_text("NF Calculated Successfully !!");
    }
    else
    {
        throw std::runtime_error("Could not calculate NF");
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
        HandleNFInsertion(formula);
    }

    return m_NFStore[ formula ];
}

void NormalForm::HandleNFInsertion(spot::formula& formula)
{
    spot::formula resultNF { ApplyOperation(formula) };
    if (!IsNFStored(formula))
    {
        m_NFStore[ formula ] = resultNF;
    }
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

bool NormalForm::AreEquals(const spot::formula& formulaA, const spot::formula& formulaB)
{
    return spot::are_equivalent(formulaA, formulaB);
}

std::vector<spot::formula> NormalForm::GetNaryResult(std::stack<spot::formula>& result)
{
    std::vector<spot::formula> opResult;

    while (!result.empty())
    {
        MoveElement(result, opResult);
    }

    return opResult;
}
void NormalForm::MoveElement(std::stack<spot::formula>& result, std::vector<spot::formula>& opResult)
{
    opResult.push_back(m_NFStore[ result.top() ]);
    result.pop();
}

void NormalForm::HandleLiteralInsertion(spot::formula& literal)
{
    if (IsNFStored(literal))
    {
        return;
    }
    else
    {
        StoreLiteralNF(literal);
    }
}
void NormalForm::StoreLiteralNF(const spot::formula& literal)
{
    m_NFStore[ literal ] = spot::formula::And({ literal, spot::formula::X(spot::formula::tt()) });
}

spot::formula NormalForm::GetElementsByOrder(spot::formula formula, std::vector<spot::formula>& elements)
{
    elements.insert(elements.begin(), formula);
    return formula.map(GetElementsByOrder, elements);
}

spot::formula NormalForm::ApplyR(spot::formula& formula)
{
    std::pair<spot::formula, spot::formula> formulasPair { formula[ static_cast<int>(Child::LEFT) ],
                                                           formula[ static_cast<int>(Child::RIGHT) ] };

    spot::formula leftFormula { CalculateRLeftFormula(m_NFStore[ formulasPair.first ],
                                                      m_NFStore[ formulasPair.second ]) };
    spot::formula nextFormula { ApplyRAnd(m_NFStore[ formulasPair.second ],
                                          GetReleaseXFormula(formulasPair.first, formulasPair.second)) };

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
    else
    {
        std::vector<spot::formula> andResult { PerformAndDNF(rightNF, next) };
        return spot::formula::Or(andResult);
    }
}
std::vector<spot::formula> NormalForm::PerformAndDNF(const spot::formula& NF, const spot::formula& nextFormula)
{
    std::vector<spot::formula> andResult;
    for (const auto& child : NF)
    {
        andResult.push_back(spot::formula::And({ child, nextFormula }));
    }
    return andResult;
}

spot::formula NormalForm::GetReleaseXFormula(const spot::formula& leftFormula, const spot::formula& rightFormula)
{
    return spot::formula::X(spot::formula::R(leftFormula, rightFormula));
}

spot::formula NormalForm::ApplyU(spot::formula& formula)
{
    std::pair<spot::formula, spot::formula> formulasPair { formula[ static_cast<int>(Child::LEFT) ],
                                                           formula[ static_cast<int>(Child::RIGHT) ] };

    spot::formula nextFormula { ApplyUAnd(m_NFStore[ formulasPair.first ],
                                          GetUntilXFormula(formulasPair.first, formulasPair.second)) };

    return spot::formula::Or({ m_NFStore[ formulasPair.second ], nextFormula });
}

spot::formula NormalForm::ApplyUAnd(spot::formula& leftNF, const spot::formula& next)
{
    if (leftNF.kind() != spot::op::Or)
    {
        return spot::formula::And({ leftNF, next });
    }
    else
    {
        std::vector<spot::formula> andResult { PerformAndDNF(leftNF, next) };
        return spot::formula::Or(andResult);
    }
}

spot::formula NormalForm::GetUntilXFormula(const spot::formula& leftFormula, const spot::formula& rightFormula)
{
    return spot::formula::X(spot::formula::U(leftFormula, rightFormula));
}

spot::formula NormalForm::ApplyAnd(spot::formula& formula)
{
    std::vector<std::vector<spot::formula>> formulasSets { ConvertFormulaToSets(formula) };

    std::vector<spot::formula> andResult { PerformeAndOnSets(formulasSets) };

    spot::formula simplifiedResult { Parser::Simplify(spot::formula::Or(andResult)) };

    return simplifiedResult.is_ff() ? spot::formula::Or(andResult) : simplifiedResult;
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
            ExtractDNFChildren(NF, andSet, true);
        }
        andResultHelper.push_back(andSet);
    }

    return andResultHelper;
}
void NormalForm::ExtractDNFChildren(const spot::formula& formula, std::vector<spot::formula>& set, bool isAndOp)
{
    if (isAndOp)
    {
        for (auto const& child : formula)
        {
            set.push_back(child);
        }
    }
    else
    {
        for (const auto& child : formula)
        {
            set.push_back(m_NFStore[ child ]);
        }
    }
}

spot::formula NormalForm::ApplyOr(spot::formula& formula)
{
    std::vector<spot::formula> orResult;

    ExtractDNFChildren(formula, orResult);

    return spot::formula::Or(orResult);
}

spot::formula NormalForm::ApplyX(spot::formula& formula)
{
    std::vector<spot::formula> nextResult;

    ExtractXChild(formula, nextResult);

    return spot::formula::Or(nextResult);
}

void NormalForm::ExtractXChild(const spot::formula& formula, std::vector<spot::formula>& nextResult)
{
    for (const auto& child : formula)
    {
        nextResult.push_back(spot::formula::X(child));
    }
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
        return CreateNFSetFromSingleElement(conn, NF);
    }
    else
    {
        return CreateNFSetFromDNF(conn, NF);
    }
}

std::set<std::pair<spot::formula, spot::formula>>
    NormalForm::CreateNFSetFromSingleElement(crow::websocket::connection& conn, spot::formula& NF) const
{
    if (AreEquals(spot::formula::ff(), NF))
    {
        return {};
    }
    else
    {
        std::pair<spot::formula, spot::formula> set { CreateNFPair(NF) };
        DisplaySet(set, conn);
        return { set };
    }
}

std::set<std::pair<spot::formula, spot::formula>> NormalForm::CreateNFSetFromDNF(crow::websocket::connection& conn,
                                                                                 spot::formula& NF)
{
    std::set<std::pair<spot::formula, spot::formula>> setsResult;
    for (auto child : NF)
    {
        std::pair<std::set<spot::formula>, std::set<spot::formula>> transformNF;
        child.traverse(TranslateNFToSet, transformNF);

        auto simplifiedSet { SimplifySet(transformNF) };
        if (!AreEquals(simplifiedSet.first, spot::formula::ff()))
        {
            setsResult.insert(simplifiedSet);
            DisplaySet(SimplifySet(transformNF), conn);
        }
    }

    return setsResult;
}

std::pair<spot::formula, spot::formula> NormalForm::CreateNFPair(spot::formula& NF) const
{
    return { std::make_pair(NF, spot::formula::X(spot::formula::tt())) };
}

std::vector<spot::formula> NormalForm::SimplifyNextFormulas(const std::set<spot::formula>& nextFormulas)
{
    std::vector<spot::formula> simplifiedFormulas { SimplifyNF(nextFormulas) };

    if (simplifiedFormulas.empty())
    {
        simplifiedFormulas.resize(nextFormulas.size());
        std::copy(nextFormulas.begin(), nextFormulas.end(), simplifiedFormulas.begin());
    }

    return simplifiedFormulas;
}

std::vector<spot::formula> NormalForm::SimplifyNF(const std::set<spot::formula>& nextFormulas) const
{
    std::vector<spot::formula> simplifiedFormulas;

    for (const auto& formula : nextFormulas)
    {
        for (const auto& storedNF : m_NFStore)
        {
            if (AreEquals(storedNF.second, formula) && !storedNF.second.is_ff())
            {
                simplifiedFormulas.push_back(storedNF.first);
            }
        }
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
    return std::make_pair(ConstructSetAndFormula(set.first), SimplifyNexts(set.second));
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
    else
    {
        return false;
    }
}

void NormalForm::CalculateElementsNF(const std::vector<spot::formula>& elements)
{
    for (auto element : elements)
    {
        m_NFStore[ element ] = NF(element);
    }
}