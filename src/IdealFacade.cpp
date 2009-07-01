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
#include "SliceAlgorithm.h"
#include "IOFacade.h"
#include "Macaulay2IOHandler.h"
#include "CanonicalCoefTermConsumer.h"
#include "error.h"
#include "FrobbyStringStream.h"
#include "SizeMaxIndepSetAlg.h"

IdealFacade::IdealFacade(bool printActions):
  Facade(printActions) {
}

void IdealFacade::deform(BigIdeal& bigIdeal) {
  beginAction("Applying generic deformation to ideal.");

  bigIdeal.deform();

  // Reduce range of exponents
  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, false);
  bigIdeal.clear();
  bigIdeal.insert(ideal);

  endAction();
}

void IdealFacade::takeRadical(BigIdeal& bigIdeal) {
  beginAction("Taking radical of ideal.");

  bigIdeal.takeRadical();

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, false);
  bigIdeal.clear();

  bigIdeal.insert(ideal, translator);

  endAction();
}

mpz_class IdealFacade::computeDimension
(const BigIdeal& bigIdeal, bool squareFreeAndMinimal) {
  size_t varCount = bigIdeal.getVarCount();
  size_t genCount = bigIdeal.getGeneratorCount();

  Ideal radical(varCount);
  Term tmp(varCount);
  for (size_t term = 0; term < genCount; ++term) {
	for (size_t var = 0; var < varCount; ++var) {
	  ASSERT(!squareFreeAndMinimal || bigIdeal[term][var] <= 1);

	  if (bigIdeal[term][var] == 0)
		tmp[var] = 0;
	  else
		tmp[var] = 1;
	}
	radical.insert(tmp);
  }
  ASSERT(!squareFreeAndMinimal || radical.isMinimallyGenerated());

  if (!squareFreeAndMinimal)
	radical.minimize();

  SizeMaxIndepSetAlg alg;
  alg.run(radical);
  return alg.getMaxIndepSetSize();
}

void IdealFacade::takeProducts(const vector<BigIdeal*>& ideals,
							   BigIdeal& ideal) {
  beginAction("Taking products.");

  size_t idealCount = ideals.size();
  for (size_t i = 0; i < idealCount; ++i) {
	ASSERT(ideals[i] != 0);

	if (!(ideal.getNames() == ideals[i]->getNames())) {
	  FrobbyStringStream errorMsg;
	  errorMsg <<
		"Taking products of ideals in rings with different variable lists.\n";
	  
	  string list;
	  ideal.getNames().toString(list);
	  errorMsg << "One ring has variables\n  " << list << ",\n";
	  
	  ideals[i]->getNames().toString(list);
	  errorMsg << "while another has variables\n  " << list <<
		".\nContact the Frobby developers if you need this functionality.";

	  reportError(errorMsg);
	}

	size_t genCount = ideals[i]->getGeneratorCount();
	size_t varCount = ideals[i]->getVarCount();

	ideal.newLastTerm();
	for (size_t t = 0; t < genCount; ++t)
	  for (size_t var = 0; var < varCount; ++var)
		ideal.getLastTermExponentRef(var) += (*ideals[i])[t][var];
  }

  endAction();
}

void IdealFacade::sortAllAndMinimize(BigIdeal& bigIdeal) {
  beginAction("Minimizing ideal.");

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, false);
  bigIdeal.clear();

  ideal.minimize();
  ideal.sortReverseLex();

  bigIdeal.insert(ideal, translator);

  endAction();
}

void IdealFacade::addPurePowers(BigIdeal& bigIdeal) {
  vector<mpz_class> lcm;
  bigIdeal.getLcm(lcm);

  vector<mpz_class> purePower(bigIdeal.getVarCount());
  for (size_t var = 0; var < bigIdeal.getVarCount(); ++var) {
	purePower[var] = lcm[var] + 1;
	if (!bigIdeal.contains(purePower))
	  bigIdeal.insert(purePower);

	ASSERT(bigIdeal.contains(purePower));

	purePower[var] = 0;
  }
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
  beginAction("Sorting variables.");

  ideal.sortVariables();

  endAction();
}

// TODO: decide what to do with this.
void IdealFacade::printAnalysis(FILE* out, BigIdeal& bigIdeal) {
  beginAction("Computing and printing analysis.");

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, false);

  fprintf(out, "is strongly generic: %s",
		  ideal.isStronglyGeneric() ? "yes" : "no");

  endAction();
}

void IdealFacade::printLcm(BigIdeal& ideal,
						   IOHandler* handler,
						   FILE* out) {
  beginAction("Computing lcm");

  vector<mpz_class> lcm;
  ideal.getLcm(lcm);

  IOFacade ioFacade(isPrintingActions());
  ioFacade.writeTerm(lcm, ideal.getNames(), handler, out);
  fputc('\n', out);

  endAction();
}
