#include "ObligationSet.h"
#include "OlgChecker.h"
#include "Parser.h"
#include <iostream>
#include <spot/tl/parse.hh>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<std::string> formulas;
    formulas.push_back("G( (a U b) & d U (e | c) & (!e | y) )");
    formulas.push_back("G(a) & F(!a)");

    Parser parser(formulas);
    std::vector<spot::formula> parsedFormulas { parser.Parse() };

    std::size_t formulaIndex = 0;
    for (const spot::formula& formula : parsedFormulas)
    {
        if (formula.is_ff())
        {
            std::cout << formulas[ formulaIndex ] << " is equivalent to false" << std::endl;
            std::cout << formulas[ formulaIndex ] << " satisfiability currently unkown!" << std::endl;
            formulaIndex++;
            continue;
        }

        std::cout << "Calculating obligations set of: " << formulas[ formulaIndex ] << std::endl;
        ObligationSet obligationSet { formula };
        obligationSet.Calculate();
        std::cout << obligationSet << std::endl;

        OlgChecker olgChecker { obligationSet };
        if (olgChecker.IsConsistent())
        {
            std::cout << formulas[ formulaIndex ] << " is Satisfiable!" << std::endl;
        }
        else
        {
            std::cout << formulas[ formulaIndex ] << " Satisfiability is still unkown!" << std::endl;
        }

        formulaIndex++;
    }
}