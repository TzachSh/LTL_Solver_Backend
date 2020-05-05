#ifndef STATE_H
#define STATE_H

#include "NormalForm.h"
#include <set>
#include <spot/tl/formula.hh>
#include <vector>

class State
{
  public:
    State(spot::formula formula);
    ~State();
    void AddTransition(const std::pair<spot::formula, State>& transition);
    spot::formula GetFormula();
    std::vector<std::pair<spot::formula, State>> GetTransitions();

  private:
    spot::formula m_formula;
    std::vector<std::pair<spot::formula, State>> m_transitions;
};

#endif