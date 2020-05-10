#include "Parser.h"

Parser::Parser() = default;
Parser::~Parser() = default;
Parser::Parser(std::vector<std::string>& formulas) : m_formulas { formulas } {}

std::vector<spot::formula> Parser::Parse()
{
    std::vector<spot::formula> parsedFormulas;
    for (const std::string& formula : m_formulas)
    {
        parsedFormulas.push_back(Parse(formula));
    }

    return parsedFormulas;
}

spot::formula Parser::Parse(const std::string& formula)
{
    spot::parsed_formula parsedFormula { spot::parse_infix_psl(formula) };
    if (!parsedFormula.f)
    {
        throw std::runtime_error("Parse " + formula + " Failed");
    }
    else
    {
        return ParseFormula(parsedFormula);
    }
}

spot::formula Parser::ParseFormula(const spot::parsed_formula& parsedFormula) const
{
    spot::formula simplifiedFormula { Simplify(parsedFormula.f) };

    if (simplifiedFormula.is_ff())
    {
        simplifiedFormula = parsedFormula.f;
    }

    return EliminateFG(simplifiedFormula);
}

spot::formula Parser::Simplify(const spot::formula& formula)
{
    spot::tl_simplifier simplifier;
    spot::formula simplifiedFormula { simplifier.simplify(formula) };

    return EliminateFG(simplifiedFormula);
}

spot::formula Parser::EliminateFG(spot::formula formula)
{
    switch (formula.kind())
    {
    case spot::op::F:
        return spot::formula::U(spot::formula::tt(), formula[ static_cast<int>(Child::LEFT) ]).map(EliminateFG);
    case spot::op::G:
        return spot::formula::R(spot::formula::ff(), formula[ static_cast<int>(Child::LEFT) ]).map(EliminateFG);
    default:
        return formula.map(EliminateFG);
    }
}