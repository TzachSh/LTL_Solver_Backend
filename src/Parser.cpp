#include "Parser.h"

Parser::Parser() = default;

Parser::Parser(std::vector<std::string>& formulas) : m_formulas { formulas } {}

Parser::~Parser() = default;

std::vector<spot::formula> Parser::Parse()
{
    std::vector<spot::formula> parsedFormulas;
    for (const std::string& formula : m_formulas)
    {
        parsedFormulas.push_back(ParseFormula(formula));
    }
    return parsedFormulas;
}

spot::formula Parser::ParseFormula(const std::string& formula)
{
    spot::parsed_formula parsedFormula { spot::parse_infix_psl(formula) };
    if (!parsedFormula.f)
    {
        throw std::runtime_error("Parse " + formula + " Failed");
    }

    // Simplify and transform to NNF
    spot::formula simplifiedFormula { Simplify(parsedFormula.f) };
    if (simplifiedFormula.is_ff())
    {
        simplifiedFormula = parsedFormula.f;
    }
    spot::formula transformedFormula { EliminateFG(simplifiedFormula) };
    return transformedFormula;
}

spot::formula Parser::Simplify(const spot::formula& formula)
{
    spot::tl_simplifier simplifier;
    spot::formula simplifiedFormula { simplifier.simplify(formula) };
    spot::formula transformedFormula { EliminateFG(simplifiedFormula) };
    return transformedFormula;
}

spot::formula Parser::EliminateFG(spot::formula formula)
{
    if (formula.is(spot::op::F))
        return spot::formula::U(spot::formula::tt(), formula[ 0 ]).map(EliminateFG);
    if (formula.is(spot::op::G))
        return spot::formula::R(spot::formula::ff(), formula[ 0 ]).map(EliminateFG);

    // Apply EliminateFG recursively on any other operator's children
    return formula.map(EliminateFG);
}