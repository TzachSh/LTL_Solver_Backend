#ifndef OBLIGATION_H
#define OBLIGATION_H

#include <memory>
#include <set>
#include <spot/tl/formula.hh>
#include <utility>
#include <vector>

class Obligation
{
  public:
    explicit Obligation(spot::formula formula);
    virtual ~Obligation();
    virtual void Calculate() = 0;

  protected:
    spot::formula m_formula;
};

#endif
