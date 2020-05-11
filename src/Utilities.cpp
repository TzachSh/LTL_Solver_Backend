#include "Utilities.h"

bool Utilities::AreEquals(const spot::formula& formulaA, const spot::formula& formulaB)
{
    return spot::are_equivalent(formulaA, formulaB);
}

spot::formula Utilities::GetElementsByOrder(spot::formula formula, std::vector<spot::formula>& elements)
{
    elements.insert(elements.begin(), formula);
    return formula.map(GetElementsByOrder, elements);
}