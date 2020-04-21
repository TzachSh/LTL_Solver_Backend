#ifndef OBLIGATIONSET_H
#define OBLIGATIONSET_H

#include "Notations.h"
#include "Obligation.h"
#include "ObligationFormula.h"
#include "crow_all.h"
#include <iostream>
#include <set>
#include <spot/tl/parse.hh>
#include <spot/tl/simplify.hh>
#include <type_traits>
#include <vector>

class NotationsStore;

class ObligationSet : public Obligation
{
  public:
    explicit ObligationSet(const spot::formula& formula);
    ~ObligationSet() override;
    void Calculate() override;
    const std::string str();
    std::vector<std::set<spot::formula>> Get() const;
    friend std::ostream& operator<<(std::ostream& out, const ObligationSet& obligationSet);

  private:
    static spot::formula Olg(spot::formula formula);

    std::vector<std::set<spot::formula>> CalculateSets(const NotationsStore& notationsStore);
    void HandleOrExtraction(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void InitializeSet(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void HandleAndExtraction(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void HandleOperation(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet,
                         NotationsStore* const& element);
    bool IsLiteralNotation(const NotationsStore* element);
};

#endif