#ifndef NORMALFORM_H
#define NORMALFORM_H

#include "Notations.h"
#include "Parser.h"
#include "crow_all.h"
#include <iostream>
#include <memory>
#include <set>
#include <spot/tl/formula.hh>
#include <spot/twaalgos/contains.hh>
#include <stack>
#include <vector>

class NormalForm
{
  public:
    explicit NormalForm(spot::formula formula);
    ~NormalForm();
    void Calculate(crow::websocket::connection& conn);
    std::set<std::pair<spot::formula, spot::formula>> ConvertToSet(crow::websocket::connection& conn);

  private:
    static spot::formula NF(spot::formula& formula);
    static void HandleLiteralInsertion(spot::formula& literal);
    static std::vector<spot::formula> GetNaryResult(std::stack<spot::formula>& result);
    static spot::formula ApplyOperation(spot::formula& formula);
    static std::vector<std::vector<spot::formula>> ConvertFormulaToSets(spot::formula& formula);
    static std::vector<spot::formula> PerformeAndOnSets(std::vector<std::vector<spot::formula>>& andResultHelper);
    static spot::formula GetUntilXFormula(const spot::formula& leftFormula, const spot::formula& rightFormula);
    static spot::formula ApplyUAnd(spot::formula& leftNF, const spot::formula& next);
    static spot::formula GetReleaseXFormula(const spot::formula& leftFormula, const spot::formula& rightFormula);
    static spot::formula ApplyRAnd(spot::formula& rightNF, const spot::formula& next);
    static spot::formula CalculateRLeftFormula(const spot::formula& leftNF, const spot::formula& rightNF);
    static spot::formula ApplyX(spot::formula& formula);
    static spot::formula ApplyOr(spot::formula& formula);
    static spot::formula ApplyAnd(spot::formula& formula);
    static spot::formula ApplyU(spot::formula& formula);
    static spot::formula ApplyR(spot::formula& formula);
    static bool IsNFStored(const spot::formula& formula);

    static bool TranslateNFToSet(spot::formula& NF,
                                 std::pair<std::set<spot::formula>, std::set<spot::formula>>& resultSet);

    static std::vector<spot::formula> PerformeAndBetweenTwoSets(const std::vector<spot::formula>& setA,
                                                                const std::vector<spot::formula>& setB);

    std::pair<spot::formula, spot::formula>
        SimplifySet(std::pair<std::set<spot::formula>, std::set<spot::formula>>& set);

    void DisplaySet(const std::pair<spot::formula, spot::formula>& set, crow::websocket::connection& conn) const;
    void CalculateElementsNF(const std::vector<spot::formula>& elements);

    spot::formula ApplyAndNextFormulas(const std::vector<spot::formula>& nextFormulas);
    spot::formula ConstructSetAndFormula(const std::set<spot::formula>& literalsSet);
    spot::formula ConstructSetNextFormula(const std::set<spot::formula>& nextFormulas);

    static void HandleNFInsertion(spot::formula& formula);
    static void MoveElement(std::stack<spot::formula>& result, std::vector<spot::formula>& opResult);
    static void StoreLiteralNF(const spot::formula& literal);
    static std::vector<spot::formula> PerformAndDNF(const spot::formula& NF, const spot::formula& nextFormula);
    static void ExtractDNFChildren(const spot::formula& formula, std::vector<spot::formula>& set, bool isAndOp = false);
    static void ExtractXChild(const spot::formula& formula, std::vector<spot::formula>& nextResult);
    std::pair<spot::formula, spot::formula> CreateNFPair(spot::formula& NF) const;
    std::set<std::pair<spot::formula, spot::formula>> CreateNFSetFromDNF(crow::websocket::connection& conn,
                                                                         spot::formula& NF);
    std::set<std::pair<spot::formula, spot::formula>> CreateNFSetFromSingleElement(crow::websocket::connection& conn,
                                                                                   spot::formula& NF) const;
    std::vector<spot::formula> TransformSetToVector(const std::set<spot::formula>& nextFormulas) const;

    static std::map<spot::formula, spot::formula> m_NFStore;
    spot::formula m_formula;
};

#endif