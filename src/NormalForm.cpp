#include "NormalForm.h"

NormalForm::NormalForm(const spot::formula& formula) : m_formula { formula } {}

NormalForm::~NormalForm() = default;

void NormalForm::Calculate() {}

spot::formula NormalForm::CalculateDF(spot::formula formula, std::set<spot::formula> disjunctsSet)
{
    if (formula.kind() == spot::op::Or)
    {
        return formula.map(CalculateDF, disjunctsSet);
    }

    disjunctsSet.insert(formula);
    return formula.map(CalculateDF, disjunctsSet);
}