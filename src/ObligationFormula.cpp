#include "ObligationFormula.h"

ObligationFormula::ObligationFormula(const spot::formula& formula) : Obligation(formula) {}

ObligationFormula::~ObligationFormula() = default;

void ObligationFormula::Calculate()
{
    m_OF = OF(m_formula);
}

spot::formula ObligationFormula::OF(spot::formula formula)
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
        return OF(formula[ static_cast<int>(Child::LEFT) ]);
    case spot::op::U:
    case spot::op::R:
        return OF(formula[ static_cast<int>(Child::RIGHT) ]);
    default:
        return formula.map(OF);
    }
}

spot::formula ObligationFormula::Get()
{
    return m_OF;
}

void ObligationFormula::Display()
{
    std::cout << m_OF << std::endl;
}