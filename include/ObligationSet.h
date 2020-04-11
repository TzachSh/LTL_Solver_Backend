#ifndef OBLIGATIONSET_H
#define OBLIGATIONSET_H

#include "Obligation.h"
#include "ObligationFormula.h"
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
    std::vector<std::set<spot::formula>> Get() const;
    friend std::ostream& operator<<(std::ostream& out, const ObligationSet& obligationSet);

  private:
    std::vector<std::set<spot::formula>> CalculateSets(const NotationsStore& notationsStore);
    static spot::formula Olg(spot::formula formula);
    static bool CreateReversePolishNotation(spot::formula& formula, NotationsStore& notationsStore);

    std::vector<std::set<spot::formula>> m_obligations;
    void HandleOrExtraction(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void InitializeSet(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void HandleAndExtraction(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet);
    void HandleOperation(std::vector<std::set<spot::formula>>& result, const std::set<spot::formula>& elementsSet,
                         NotationsStore* const& element);
    bool IsLiteralNotation(const NotationsStore* element);
    spot::formula ConvertToFormula(NotationsStore* element);
    spot::op ConvertToOperator(NotationsStore* element);
};

class NotationsStore
{
  public:
    virtual ~NotationsStore()
    {
        for (auto& notation : m_notations)
        {
            delete notation;
        }
    }
    virtual void Display() const
    {
        for (const auto& element : m_notations)
        {
            element->Display();
        }
    };
    void AddElement(NotationsStore* element)
    {
        m_notations.insert(m_notations.begin(), element);
    }
    std::vector<NotationsStore*> GetElements() const
    {
        return m_notations;
    }

    template <typename Base, typename T> static bool InstanceOf(const T* ptr)
    {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }

    template <typename S, typename T> static T Convert(NotationsStore* element)
    {
        return dynamic_cast<S*>(element)->Get();
    }

  private:
    std::vector<NotationsStore*> m_notations;
};

class NotationFormula : public NotationsStore
{
  public:
    explicit NotationFormula(spot::formula& formula) : m_formula { formula } {}
    spot::formula Get() const
    {
        return m_formula;
    };

  private:
    spot::formula m_formula;
};

class NotationOp : public NotationsStore
{
  public:
    explicit NotationOp(const spot::op& op) : m_operator { op } {};
    spot::op Get() const
    {
        return m_operator;
    };

  private:
    spot::op m_operator;
};

#endif