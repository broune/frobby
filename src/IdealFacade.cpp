/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "IdealFacade.h"

#include "BigIdeal.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include "IOHandler.h"
#include "HilbertSliceAlgorithm.h"

#include "Macaulay2IOHandler.h"
#include "CanonicalCoefTermConsumer.h"

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
									 const string& format) {
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

void IdealFacade::printHilbertSeries(const BigIdeal& bigIdeal,
									 bool canonicalize, FILE* out) {
  beginAction("Computing and printing Hilbert series.");

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal);
  ideal.minimize();

  CoefTermConsumer* consumer;
  if (out == 0)
	consumer = new NullCoefTermConsumer();
  else
	consumer = Macaulay2IOHandler::createCoefTermWriter(stdout, &translator);

  if (canonicalize)
	consumer = new CanonicalCoefTermConsumer(consumer, ideal.getVarCount());

  HilbertSliceAlgorithm algo;
  algo.setConsumer(consumer);
  algo.run(ideal);

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
