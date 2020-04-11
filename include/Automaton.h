#ifndef AUTOMATON_H
#define AUTOMATON_H

#include "NormalForm.h"
#include "State.h"
#include <queue>
#include <spot/tl/formula.hh>
#include <vector>

class Automaton
{
  public:
    Automaton(spot::formula formula);
    ~Automaton();
    void Display();

  private:
    spot::formula GetFormula(const std::pair<spot::formula, spot::formula>& transition);
    spot::formula m_initialFormula;
    std::vector<State> m_states;
};

#endif
