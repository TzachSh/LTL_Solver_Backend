#ifndef C___PROJECT_NOTATIONS_H
#define C___PROJECT_NOTATIONS_H

#include <spot/tl/parse.hh>
#include <spot/tl/simplify.hh>

class NotationsStore
{
  public:
    virtual ~NotationsStore();
    void AddElement(NotationsStore* element);
    std::vector<std::shared_ptr<NotationsStore>> GetElements() const;

    template <typename Base, typename T> static bool InstanceOf(const T* ptr);
    template <typename S, typename T> static T Convert(NotationsStore* element);
    static bool CreateReversePolishNotation(spot::formula& formula, NotationsStore& notationsStore);
    static bool IsLiteralNotation(NotationsStore* element);
    static spot::formula ConvertToFormula(NotationsStore* element);
    static spot::op ConvertToOperator(NotationsStore* element);

    friend std::ostream& operator<<(std::ostream& out, const NotationsStore& notationsStore);

  private:
    std::vector<std::shared_ptr<NotationsStore>> m_notations;
};

class NotationFormula : public NotationsStore
{
  public:
    explicit NotationFormula(spot::formula& formula);
    spot::formula Get() const;
    friend std::ostream& operator<<(std::ostream& out, const NotationFormula& notationFormula);

  private:
    spot::formula m_formula;
};

class NotationOp : public NotationsStore
{
  public:
    explicit NotationOp(const spot::op& op);
    spot::op Get() const;
    friend std::ostream& operator<<(std::ostream& out, const NotationOp& notationOp);

  private:
    spot::op m_operator;
};
#endif // C___PROJECT_NOTATIONS_H
