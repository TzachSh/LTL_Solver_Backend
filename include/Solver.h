#ifndef SOLVER_H
#define SOLVER_H

#include "Formula.h"
#include "OlgChecker.h"
#include "WeakSatisfactionChecker.h"

class Solver {
public:
  Solver(/* args */);
  ~Solver();
  bool Solve(const Formula& formula);

private:
  /* data */
};

#endif
