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
#include "GenerateDataFacade.h"

#include "randomDataGenerators.h"
#include "error.h"

GenerateDataFacade::GenerateDataFacade(bool printActions):
  Facade(printActions) {
}

void GenerateDataFacade::generateListIdeal(BigIdeal& ideal,
										   size_t variableCount) {
  beginAction("Generating list ideal.");

  ::generateLinkedListIdeal(ideal, variableCount);

  endAction();
}

void GenerateDataFacade::generateKingChessIdeal(BigIdeal& ideal,
												unsigned int rowsAndColumns) {
  beginAction("Generating king ideal.");

  ::generateKingChessIdeal(ideal, rowsAndColumns);

  endAction();
}

void GenerateDataFacade::generateKnightChessIdeal
(BigIdeal& ideal,
 unsigned int rowsAndColumns) {
  beginAction("Generating knight ideal.");

  ::generateKnightChessIdeal(ideal, rowsAndColumns);

  endAction();
}

void GenerateDataFacade::generateIdeal(BigIdeal& ideal,
									   size_t exponentRange,
									   size_t variableCount,
									   size_t generatorCount) {
  beginAction("Generating random monomial ideal.");

  bool fullSize =
	::generateRandomIdeal(ideal, exponentRange, variableCount, generatorCount);

  if (!fullSize)
	displayNote("Generated ideal has fewer minimal "
				"generators than requested.\n");

  endAction();  
}

void GenerateDataFacade::
generateFrobeniusInstance(vector<mpz_class>& instance) {
  beginAction("Generating random Frobenius instance.");

  ::generateRandomFrobeniusInstance(instance);

  endAction();
}
