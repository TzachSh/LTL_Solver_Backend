#ifndef C___PROJECT_UTILITIES_H
#define C___PROJECT_UTILITIES_H

#include <spot/tl/formula.hh>
#include <spot/twaalgos/contains.hh>

namespace Utilities {
bool AreEquals(const spot::formula& formulaA, const spot::formula& formulaB);
spot::formula GetElementsByOrder(spot::formula formula, std::vector<spot::formula>& elements);
bool CalculateDepth(spot::formula formula, std::map<spot::formula, int>& depthMap);
} // namespace Utilities

#endif // C___PROJECT_UTILITIES_H
