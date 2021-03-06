#ifndef OBLIGATIONSET_H
#define OBLIGATIONSET_H

#include "Constants.h"
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
    std::string str();
    std::vector<std::set<spot::formula>> Get() const;
    friend std::ostream& operator<<(std::ostream& out, const ObligationSet& obligationSet);
    static void ExtractOlgSetToStream(std::ostream& out, const std::set<spot::formula>& olgSet);

  private:
    std::vector<std::set<spot::formula>> CalculateSets(const NotationsStore& notationsStore);
    void HandleOr(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void InitializeSet(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void HandleAndExtraction(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void HandleOperation(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet,
                         NotationsStore* const& element);

    std::vector<std::set<spot::formula>> m_obligations;
    void HandleLiteralInsertion(const NotationsStore& notationsStore, std::vector<std::set<spot::formula>>& result,
                                std::set<spot::formula>& elementsSet,
                                const std::shared_ptr<NotationsStore>& element) const;
    void HandleAnd(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
};

#endif