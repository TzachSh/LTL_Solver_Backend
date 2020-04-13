#include "Notations.h"

NotationsStore::~NotationsStore() {}

void NotationsStore::AddElement(NotationsStore* element)
{
    m_notations.insert(m_notations.begin(), std::shared_ptr<NotationsStore>(element));
}

std::vector<std::shared_ptr<NotationsStore>> NotationsStore::GetElements() const
{
    return m_notations;
}

template <typename Base, typename T> bool NotationsStore::InstanceOf(const T* ptr)
{
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

template <typename S, typename T> T NotationsStore::Convert(NotationsStore* element)
{
    return dynamic_cast<S*>(element)->Get();
}

NotationFormula::NotationFormula(spot::formula& formula) : m_formula { formula } {}

spot::formula NotationFormula::Get() const
{
    return m_formula;
};

NotationOp::NotationOp(const spot::op& op) : m_operator { op } {};

spot::op NotationOp::Get() const
{
    return m_operator;
};

spot::formula NotationsStore::ConvertToFormula(NotationsStore* element)
{
    return NotationsStore::Convert<NotationFormula, spot::formula>(element);
}

spot::op NotationsStore::ConvertToOperator(NotationsStore* element)
{
    return NotationsStore::Convert<NotationOp, spot::op>(element);
}

bool NotationsStore::IsLiteralNotation(NotationsStore* element)
{
    if (NotationsStore::InstanceOf<NotationFormula>(element))
    {
        spot::formula formula { NotationsStore::ConvertToFormula(element) };
        return (formula.is_literal() || formula.is_tt() || formula.is_ff());
    }

    return false;
}

bool NotationsStore::CreateReversePolishNotation(spot::formula& formula, NotationsStore& notationsStore)
{
    if (formula.is_literal() || formula.is_tt() || formula.is_ff())
    {
        notationsStore.AddElement(new NotationFormula(formula));
    }
    else
    {
        notationsStore.AddElement(new NotationOp(formula.kind()));
    }
    return formula.is_literal();
}

std::ostream& operator<<(std::ostream& out, const NotationFormula& notationFormula)
{
    out << notationFormula.Get();
    return out;
}

std::ostream& operator<<(std::ostream& out, const NotationOp& notationOp)
{
    switch (notationOp.Get())
    {
    case spot::op::And:
        out << "&";
        break;
    case spot::op::Or:
        out << "|";
        break;
    case spot::op::U:
        out << "U";
        break;
    case spot::op::R:
        std::cout << "R";
        break;
    case spot::op::X:
        std::cout << "X";
        break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const NotationsStore& notationsStore)
{
    for (const auto& element : notationsStore.GetElements())
    {
        bool isFormula { NotationsStore::InstanceOf<NotationFormula>(element.get()) };
        if (isFormula)
        {
            out << NotationsStore::ConvertToFormula(element.get());
        }
        else
        {
            NotationOp notationOp { NotationsStore::ConvertToOperator(element.get()) };
            out << notationOp;
        }
    }
    out << std::endl;

    return out;
}
