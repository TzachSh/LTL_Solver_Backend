#include "ObligationSet.h"

ObligationSet::ObligationSet(const spot::formula& formula) : Obligation(formula) {}

ObligationSet::~ObligationSet() = default;

bool ObligationSet::CreateReversePolishNotation(spot::formula& formula, NotationsStore& notationsStore)
{
    if (formula.is_literal())
    {
        notationsStore.AddElement(new NotationFormula(formula));
    }
    else
    {
        notationsStore.AddElement(new NotationOp(formula.kind()));
    }
    return formula.is_literal();
}

spot::formula ObligationSet::ConvertToFormula(NotationsStore* element)
{
    return NotationsStore::Convert<NotationFormula, spot::formula>(element);
}

spot::op ObligationSet::ConvertToOperator(NotationsStore* element)
{
    return NotationsStore::Convert<NotationOp, spot::op>(element);
}

std::vector<std::set<spot::formula>> ObligationSet::CalculateSets(const NotationsStore& notationsStore)
{
    std::vector<std::set<spot::formula>> result;
    std::set<spot::formula> elementsSet;
    for (auto& element : notationsStore.GetElements())
    {
        if (IsLiteralNotation(element))
        {
            elementsSet.insert(ConvertToFormula(element));
        }
        else
        {
            HandleOperation(result, elementsSet, element.get());
            elementsSet.clear();
        }
    }

    return result;
}
void ObligationSet::HandleOperation(std::vector<std::set<spot::formula>>& result,
                                    const std::set<spot::formula>& elementsSet, NotationsStore* const& element)
{
    switch (ConvertToOperator(element))
    {
    case spot::op::Or:
    {
        HandleOrExtraction(result, elementsSet);
        break;
    }
    case spot::op::And:
    {
        if (result.empty())
        {
            InitializeSet(result, elementsSet);
        }
        else
        {
            HandleAndExtraction(result, elementsSet);
        }
    }
    }
}

void ObligationSet::HandleAndExtraction(std::vector<std::set<spot::formula>>& result,
                                        const std::set<spot::formula>& elementsSet)
{
    for (const auto& literal : elementsSet)
    {
        for (auto& set : result)
        {
            set.insert(literal);
        }
    }
}
void ObligationSet::InitializeSet(std::vector<std::set<spot::formula>>& result,
                                  const std::set<spot::formula>& elementsSet)
{
    for (const auto& literal : elementsSet)
    {
        result.push_back({ literal });
    }
}
void ObligationSet::HandleOrExtraction(std::vector<std::set<spot::formula>>& result,
                                       const std::set<spot::formula>& elementsSet)
{
    for (const auto& literal : elementsSet)
    {
        result.push_back({ literal });
    }
}

void ObligationSet::Calculate()
{
    spot::formula olgFormula { Olg(m_formula) };
    NotationsStore notationsStore;

    olgFormula.traverse(CreateReversePolishNotation, notationsStore);
    m_obligations = CalculateSets(notationsStore);
}

spot::formula ObligationSet::Olg(spot::formula formula)
{
    if (formula.is_tt() || formula.is_ff() || formula.is_literal())
    {
        return formula;
    }
    switch (formula.kind())
    {
    case spot::op::Not:
        return formula;
    case spot::op::X:
        return formula[ static_cast<int>(Child::LEFT) ].map((Olg));
    case spot::op::U:
    case spot::op::R:
        return formula[ static_cast<int>(Child::RIGHT) ].map(Olg);
    case spot::op::Or:
    case spot::op::And:
    default:
        return formula.map(Olg);
    }
}

bool ObligationSet::IsLiteralNotation(const NotationsStore* element)
{
    return NotationsStore::InstanceOf<NotationFormula>(element);
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
