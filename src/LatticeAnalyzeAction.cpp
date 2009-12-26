/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

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
#include "LatticeAnalyzeAction.h"

#include "SatBinomIdeal.h"
#include "IOFacade.h"
#include "Scanner.h"
#include "IOHandler.h"
#include "DataType.h"
#include "BigIdeal.h"
#include "MsmStrategy.h"
#include "TermTranslator.h"
#include "TranslatingTermConsumer.h"
#include "DebugStrategy.h"

#include <algorithm>

LatticeAnalyzeAction::LatticeAnalyzeAction():
  Action
(staticGetName(),
 "Display information about the input ideal.",
 "This action is not ready for use.\n\n"
 "Display information about input Grobner basis of lattice.",
 false),

  _io(DataType::getSatBinomIdealType(), DataType::getMonomialIdealType()) {
}

void LatticeAnalyzeAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

bool LatticeAnalyzeAction::displayAction() const {
  return false;
}

void LatticeAnalyzeAction::perform() {
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  IOFacade ioFacade(_printActions);
  SatBinomIdeal ideal;
  ioFacade.readSatBinomIdeal(in, ideal);

  bool generic = ideal.hasZeroEntry();

  mpz_class triCount;
  ideal.getDoubleTriangleCount(triCount);
  fprintf(stdout, "%u neighbors, ", (unsigned int)ideal.getGeneratorCount());
  gmp_fprintf(stdout, "%Zd double triangles, ", triCount.get_mpz_t());

  if (!generic)
	fputs("generic.\n", stdout);
  else
	fputs("not generic.\n", stdout);

  if (triCount == 0)
	exit(1);
  if (generic)
	exit(2);
}

const char* LatticeAnalyzeAction::staticGetName() {
  return "latanal";
}
