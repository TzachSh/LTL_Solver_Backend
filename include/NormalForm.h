#ifndef NORMALFORM_H
#define NORMALFORM_H


#include "Literal.h"
#include <memory>
#include <vector>
#include <set>

typedef std::vector<std::pair<std::set<Literal>, Formula>> NFDatatype;

class NormalForm
{
  public:
    ~NormalForm();
    void Calculate();

  private:
    std::shared_ptr<Formula> m_formula;
    NFDatatype m_store;
};

#endif