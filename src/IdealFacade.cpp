#include "stdinc.h"
#include "IdealFacade.h"

#include "BigIdeal.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include "IOHandler.h"

IdealFacade::IdealFacade(bool printActions):
  Facade(printActions) {
}

void IdealFacade::deform(BigIdeal& bigIdeal) {
  beginAction("Applying generic deformation to ideal.");

  bigIdeal.deform();

  // Reduce range of exponents
  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, true);
  bigIdeal.clear();
  bigIdeal.insert(ideal);

  endAction();
}

void IdealFacade::takeRadical(BigIdeal& bigIdeal) {
  beginAction("Taking radical of ideal.");

  bigIdeal.takeRadical();

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, true);
  bigIdeal.clear();

  ideal.minimize();
  ideal.sortReverseLex();

  bigIdeal.insert(ideal, translator);

  endAction();
}

void IdealFacade::sortAllAndMinimize(BigIdeal& bigIdeal) {
  beginAction("Minimizing ideal.");

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, true);
  bigIdeal.clear();

  ideal.minimize();
  ideal.sortReverseLex();

  bigIdeal.insert(ideal, translator);

  endAction();
}

void IdealFacade::sortAllAndMinimize(BigIdeal& bigIdeal, FILE* out,
									 const char* format) {
  beginAction("Minimizing and writing ideal.");

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, true);
  bigIdeal.clear();

  ideal.minimize();
  ideal.sortReverseLex();

  IOHandler* handler = IOHandler::getIOHandler(format);
  ASSERT(handler != 0);
  
  handler->writeIdeal(out, ideal, &translator);

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

void IdealFacade::printAnalysis(FILE* out, BigIdeal& bigIdeal) {
  beginAction("Computing and printing analysis.");

  fprintf(out, "%u generators\n", (unsigned int)bigIdeal.getGeneratorCount());
  fprintf(out, "%u variables\n", (unsigned int)bigIdeal.getVarCount());

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, true);

  fprintf(out, "is strongly generic: %s",
		  ideal.isStronglyGeneric() ? "yes" : "no");

/*
TODO:

CHEAP
square-free
canonical
partition
lcm exponent vector
gcd exponent vector
sparsity

EXPENSIVE
minimized

MORE EXPENSIVE
weakly generic (could this be done efficiently?)

VERY EXPENSIVE
size of irreducible decomposition
strongly cogeneric
dimension
degree

EVEN MORE EXPENSIVE
weakly cogeneric

EVEN EVEN MORE EXPENSIVE
self Alexander dual

*/

  endAction();
}

void IdealFacade::printLcm(FILE* out, BigIdeal& ideal) {
  beginAction("Computing lcm");

  // TODO: integrate this with the regular IO system
  vector<mpz_class> lcm;
  ideal.getLcm(lcm);
  if (lcm == vector<mpz_class>(lcm.size()))
	fputs("1\n", out);
  else {
	const char* pre = "";
	for (size_t var = 0; var < ideal.getVarCount(); ++var) {
	  if (lcm[var] == 0)
		continue;
	  gmp_fprintf(out, lcm[var] == 1 ? "%s%s" : "%s%s^%Zd",
				  pre,
				  ideal.getNames().getName(var).c_str(),
				  lcm[var].get_mpz_t());
	  pre = "*";
	}
	fputc('\n', out);
  }
  
  endAction();
}
