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

  ideal.minimize();
  ideal.sortReverseLex();

  bigIdeal.insert(ideal, translator);

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
