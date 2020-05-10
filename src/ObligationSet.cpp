#include "ObligationSet.h"

ObligationSet::ObligationSet(const spot::formula& formula) : Obligation(formula) {}

ObligationSet::~ObligationSet() = default;

std::vector<std::set<spot::formula>> ObligationSet::CalculateSets(const NotationsStore& notationsStore)
{
    std::vector<std::set<spot::formula>> result;

    std::set<spot::formula> elementsSet;
    for (auto& element : notationsStore.GetElements())
    {
        if (NotationsStore::IsLiteralNotation(element.get()))
        {
            HandleLiteralInsertion(notationsStore, result, elementsSet, element);
        }
        else
        {
            HandleOperation(result, elementsSet, element.get());
            elementsSet.clear();
        }
    }

    return result;
}

void ObligationSet::HandleLiteralInsertion(const NotationsStore& notationsStore,
                                           std::vector<std::set<spot::formula>>& result,
                                           std::set<spot::formula>& elementsSet,
                                           const std::shared_ptr<NotationsStore>& element) const
{
    spot::formula literal { NotationsStore::ConvertToFormula(element.get()) };

    constexpr int MIN_OP_ARGS = 2;
    if (notationsStore.GetElements().size() >= MIN_OP_ARGS)
    {
        elementsSet.insert(literal);
    }
    else
    {
        result.push_back({ literal });
    }
}

void ObligationSet::HandleOperation(std::vector<std::set<spot::formula>>& result,
                                    const std::set<spot::formula>& elementsSet, NotationsStore* const& element)
{
    switch (NotationsStore::ConvertToOperator(element))
    {
    case spot::op::Or:
        HandleOr(result, elementsSet);
        break;
    case spot::op::And:
        HandleAnd(result, elementsSet);
        break;
    }
}
void ObligationSet::HandleAnd(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet)
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
    std::set<spot::formula> initSet;

    for (const auto& literal : elementsSet)
    {
        initSet.insert(literal);
    }

    result.push_back(initSet);
}

void ObligationSet::HandleOr(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet)
{
    for (const auto& literal : elementsSet)
    {
        result.push_back({ literal });
    }
}

void ObligationSet::Calculate()
{
    spot::formula olgFormula { ObligationFormula::OF(m_formula) };

    NotationsStore notationsStore;
    olgFormula.traverse(NotationsStore::CreateReversePolishNotation, notationsStore);

    m_obligations = CalculateSets(notationsStore);
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
        ObligationSet::ExtractOlgSetToStream(out, obligation);
    }
    out << "}";

    return out;
}

std::string ObligationSet::str()
{
    std::ostringstream stringStream;

    stringStream << "Obligations Set: { ";
    for (const auto& obligation : m_obligations)
    {
        ObligationSet::ExtractOlgSetToStream(stringStream, obligation);
    }
    stringStream << "}";

    return stringStream.str();
}

void ObligationSet::ExtractOlgSetToStream(std::ostream& out, const std::set<spot::formula>& olgSet)
{
    out << "{ ";
    for (const auto& literal : olgSet)
    {
        out << literal << " ";
    }
    out << "} ";
}