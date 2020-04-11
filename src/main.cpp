#include "ObligationSet.h"
#include "OlgChecker.h"
#include "Parser.h"
#include <include/Automaton.h>
#include <include/NormalForm.h>
#include <iostream>
#include <spot/tl/parse.hh>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<std::string> formulas;
    // formulas.push_back("G( (a U b) & d U (e | c) & (!e | y) )");
    // formulas.push_back("G( (a U b) & X( a ) & (d U (e | c & (d | f))) | ( y R ( j | k | n )) )");
    // formulas.push_back("G( a U (c & b) & f R (j & (k | c))");
    formulas.push_back("G(a U b & (c U (e | d))");

    Parser parser(formulas);
    std::vector<spot::formula> parsedFormulas { parser.Parse() };

    std::size_t formulaIndex = 0;
    for (spot::formula& formula : parsedFormulas)
    {
        if (formula.is_ff())
        {
            std::cout << formulas[ formulaIndex ] << " is equivalent to false" << std::endl;
            std::cout << formulas[ formulaIndex ] << " satisfiability currently unknown!" << std::endl;
            formulaIndex++;
            continue;
        }

        std::cout << "Calculating obligations set of: " << formulas[ formulaIndex ] << std::endl;
        ObligationSet obligationSet { formula };
        obligationSet.Calculate();
        std::cout << obligationSet << std::endl;

        NormalForm normalForm { formula };

        OlgChecker olgChecker { obligationSet };
        if (olgChecker.IsConsistent())
        {
            std::cout << formulas[ formulaIndex ] << " is Satisfiable!" << std::endl;
        }
        else
        {
            std::cout << formulas[ formulaIndex ] << " Satisfiability is still unknown!" << std::endl;
            // NormalForm normalForm { formula };
            // normalForm.Calculate();
        }

        formulaIndex++;
    }
}