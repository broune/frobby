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
#ifndef RANDOM_DATA_GENERATORS_GUARD
#define RANDOM_DATA_GENERATORS_GUARD

#include <vector>

class BigIdeal;

void generateLinkedListIdeal(BigIdeal& ideal, unsigned int variableCount);

void generateKingChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns);
void generateKnightChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns);

bool generateRandomIdeal(BigIdeal& bigIdeal,
						 unsigned int exponentRange,
						 unsigned int variableCount,
						 unsigned int generatorCount);

void generateRandomFrobeniusInstance(vector<mpz_class>& degrees);

#endif
