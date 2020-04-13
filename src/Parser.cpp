#include "Parser.h"

Parser::Parser() = default;

Parser::Parser(std::vector<std::string> &formulas) : m_formulas{formulas} {}

Parser::~Parser() = default;

std::vector<spot::formula> Parser::Parse()
{
    std::vector<spot::formula> parsedFormulas;
    for (const std::string& formula : m_formulas)
    {
        spot::parsed_formula parsedFormula { spot::parse_infix_psl(formula) };
        if (parsedFormula.f)
        {
            // Simplify and transform to NNF
            spot::formula simplifiedFormula { Simplify(parsedFormula.f) };
            if (simplifiedFormula.is_ff())
            {
                simplifiedFormula = parsedFormula.f;
            }
            spot::formula transformedFormula { EliminateFG(simplifiedFormula) };
            std::cout << transformedFormula << std::endl;
            parsedFormulas.push_back(transformedFormula);
        }
        else
        {
            throw std::runtime_error("Parse failed");
        }
    }
    return parsedFormulas;
}

spot::formula Parser::Simplify(const spot::formula& formula)
{
    spot::tl_simplifier simplifier;
    spot::formula simplifiedFormula { simplifier.simplify(formula) };
    return simplifiedFormula;
}

spot::formula Parser::EliminateFG(spot::formula formula)
{
    if (formula.is(spot::op::F))
        return spot::formula::U(spot::formula::tt(), formula[0]);
    if (formula.is(spot::op::G))
        return spot::formula::R(spot::formula::ff(), formula[0]);

    // Not needed to transform subformulas without F or G
    if (formula.is_sugar_free_ltl())
        return formula;
    // Apply EliminateFG recursively on any other operator's children
    return formula.map(EliminateFG);
}