#include "stdinc.h"
#include "IdealFacade.h"

#include "BigIdeal.h"
#include "Ideal.h"
#include "TermTranslator.h"

IdealFacade::IdealFacade(bool printActions):
  Facade(printActions) {
}

void IdealFacade::minimize(BigIdeal& bigIdeal) {
  beginAction("Minimizing ideal.");

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal);
  bigIdeal.clear();

  ideal.minimize();

  bigIdeal.insert(ideal, translator);

  endAction();
}

void IdealFacade::sortGeneratorsUnique(BigIdeal& ideal) {
  beginAction("Sorting generators and removing duplicates.");

  ideal.sortGeneratorsUnique();

  endAction();
}

void IdealFacade::sortGenerators(BigIdeal& ideal) {
  beginAction("Sorting generators.");

  ideal.sortGenerators();

  endAction();
}

void IdealFacade::sortVariables(BigIdeal& ideal) {
  beginAction("Sorting generators.");

  ideal.sortVariables();

  endAction();
}

void IdealFacade::printAnalysis(ostream& out, BigIdeal& ideal) {
  beginAction("Computing and printing analyze.");

  out << ideal.getGeneratorCount() << " generators\n"
      << ideal.getVarCount() << " variables\n"
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
