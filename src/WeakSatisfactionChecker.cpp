#include "WeakSatisfactionChecker.h"

WeakSatisfactionChecker::WeakSatisfactionChecker(spot::formula& condition, const spot::formula& of,
                                                 crow::websocket::connection& conn)
    : m_of { of }
{
    std::cout << "Condition: " << condition << std::endl;
    std::cout << "Obligation Formula: " << of << std::endl;

    condition.traverse(StoreConditionLiterals, m_conditionLiterals);
}

WeakSatisfactionChecker::~WeakSatisfactionChecker() = default;

std::string WeakSatisfactionChecker::DisplayConditionAsSet(const spot::formula& condition)
{
    std::ostringstream stringStream;
    if (condition.kind() != spot::op::And)
    {
        stringStream << "{ " << condition << " }";
        return stringStream.str();
    }

    stringStream << "{ ";
    for (const auto& child : condition)
    {
        stringStream << child << " ";
    }
    stringStream << "}";

    return stringStream.str();
}

bool WeakSatisfactionChecker::Check()
{
    std::vector<spot::formula> elements { m_of };
    m_of.map(NormalForm::GetElementsByOrder, elements);

    StoreElementsSatisfactionInfo(elements);

    return m_satisfactionInfo[ m_of ];
}

bool WeakSatisfactionChecker::StoreConditionLiterals(spot::formula formula, std::vector<spot::formula>& literalsStore)
{
    if (formula.is_literal() || formula.is_tt() || formula.is_ff())
    {
        literalsStore.push_back(formula);
        return true;
    }

    return false;
}

void WeakSatisfactionChecker::StoreElementsSatisfactionInfo(const std::vector<spot::formula>& elements)
{
    for (auto element : elements)
    {
        SaveInfo(element);
    }
}

void WeakSatisfactionChecker::SaveInfo(spot::formula& formula)
{
    if (formula.is_literal() || formula.is_tt() || formula.is_ff())
    {
        m_satisfactionInfo[ formula ] = IsExists(m_conditionLiterals, formula);
    }
    else
    {
        m_satisfactionInfo[ formula ] = ApplyOperation(formula);
    }
}

spot::formula WeakSatisfactionChecker::IsExists(const std::vector<spot::formula>& conditionLiterals,
                                                const spot::formula& literal)
{
    bool isConditionContainsElement { std::find(conditionLiterals.begin(), conditionLiterals.end(), literal) !=
                                      conditionLiterals.end() };
    return isConditionContainsElement ? spot::formula::tt() : spot::formula::ff();
}

spot::formula WeakSatisfactionChecker::ApplyOperation(const spot::formula& formula)
{
    switch (formula.kind())
    {
    case spot::op::Or:
        return ApplyOr(formula);
    case spot::op::And:
        return ApplyAnd(formula);
    default:
        return formula;
    }
}

spot::formula WeakSatisfactionChecker::ApplyOr(const spot::formula& formula)
{
    std::vector<spot::formula> literalsInfo { GetFormulaLiteralsInfo(formula) };
    return spot::formula::Or(literalsInfo);
}

spot::formula WeakSatisfactionChecker::ApplyAnd(const spot::formula& formula)
{
    std::vector<spot::formula> literalsInfo { GetFormulaLiteralsInfo(formula) };
    return spot::formula::And(literalsInfo);
}

std::vector<spot::formula> WeakSatisfactionChecker::GetFormulaLiteralsInfo(const spot::formula& formula)
{
    std::vector<spot::formula> result;

    for (const auto& element : formula)
    {
        result.push_back(m_satisfactionInfo[ element ]);
    }

    return result;
}
