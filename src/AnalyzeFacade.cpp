#include "stdinc.h"
#include "AnalyzeFacade.h"

#include "BigIdeal.h"

AnalyzeFacade::AnalyzeFacade(bool printActions):
  Facade(printActions) {
}

void AnalyzeFacade::printAnalysis(ostream& out, BigIdeal& ideal) {
  beginAction("Computing and printing analyze.");

  out << ideal.size() << " generators\n"
      << ideal.getNames().getVarCount() << " variables\n"
      << "TODO: implement more analyze" << endl;


/*
TODO:

CHEAP
square-free
weakly generic
canonical
partition
lcm exponent vector
gcd exponent vector
sparsity

EXPENSIVE
minimized

MORE EXPENSIVE
strongly generic

VERY EXPENSIVE
size of irreducible decomposition
cogeneric
dimension
degree

*/

  endAction();
}
