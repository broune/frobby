#include "stdinc.h"
#include "IdealFacade.h"

#include "BigIdeal.h"
#include "Ideal.h"
#include "TermTranslator.h"

#include <cstdio>

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

void IdealFacade::printAnalysis(FILE* out, BigIdeal& ideal) {
  beginAction("Computing and printing analyze.");

  fprintf(out, "%u generators\n", (unsigned int)ideal.getGeneratorCount());
  fprintf(out, "%u variables\n", (unsigned int)ideal.getVarCount());
  fprintf(out, "TODO: implement more analyses.\n");
  fflush(out);

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
