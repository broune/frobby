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
#include "randomDataGenerators.h"

#include "BigIdeal.h"
#include "Ideal.h"
#include "Term.h"
#include "error.h"
#include "FrobbyStringStream.h"

#include <limits>

void generateLinkedListIdeal(BigIdeal& ideal, unsigned int variableCount) {
  VarNames names(variableCount);
  ideal.clearAndSetNames(variableCount);
  ideal.reserve(variableCount);
  for (size_t var = 1; var < variableCount; ++var) {
	ideal.newLastTerm();
	ideal.getLastTermExponentRef(var) = 1;
	ideal.getLastTermExponentRef(var - 1) = 1;
  }
}

void generateChessIdeal(BigIdeal& bigIdeal,
						unsigned int rowCount,
						unsigned int columnCount,
						int deltaRow[],
						int deltaColumn[],
						size_t deltaCount) {
  if (mpz_class(rowCount) * mpz_class(columnCount) >
	  numeric_limits<size_t>::max())
	reportError("Number of positions on requested chess board too large.");

  // Generate names
  VarNames names;
  for (unsigned int row = 0; row < rowCount; ++row) {
	for (unsigned int column = 0; column < columnCount; ++column) {
	  FrobbyStringStream name;
	  name << 'r' << (row + 1) << 'c' << (column + 1);
	  names.addVar(name);
	}
  }
  bigIdeal.clearAndSetNames(names);
  Ideal ideal(bigIdeal.getVarCount());

  // Generate ideal
  for (unsigned int row = 0; row < rowCount; ++row) {
	for (unsigned int column = 0; column < columnCount; ++column) {
	  for (size_t delta = 0; delta < deltaCount; ++delta) {
		// Check that the target position is within the board.
		
		if (deltaRow[delta] == numeric_limits<int>::min() ||
			(deltaRow[delta] < 0 &&
			 row < (unsigned int)-deltaRow[delta]) ||
			(deltaRow[delta] > 0 &&
			 rowCount - row <= (unsigned int)deltaRow[delta]))
		  continue;

		if (deltaColumn[delta] == numeric_limits<int>::min() ||
			(deltaColumn[delta] < 0 &&
			 column < (unsigned int)-deltaColumn[delta]) ||
			(deltaColumn[delta] > 0 &&
			 columnCount - column <= (unsigned int)deltaColumn[delta]))
		  continue;

		Term chessMove(ideal.getVarCount());
		chessMove[row * columnCount + column] = 1;

		unsigned int targetRow = row + deltaRow[delta];
		unsigned int targetColumn = column + deltaColumn[delta];
		ASSERT(targetRow < rowCount);
		ASSERT(targetColumn < columnCount);

		chessMove[targetRow * columnCount + targetColumn] = 1;
		ideal.insert(chessMove);
	  }
	}
  }

  ideal.sortReverseLex();
  bigIdeal.insert(ideal);
}

void generateKingChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns) {
  int deltaRow[]    = {-1, 0, 1, 1}; // the other moves follow by symmetry
  int deltaColumn[] = { 1, 1, 1, 0};
  ASSERT(sizeof(deltaRow) == sizeof(deltaColumn));

  size_t deltaCount = sizeof(deltaRow) / sizeof(int);

  generateChessIdeal(ideal, rowsAndColumns, rowsAndColumns,
					 deltaRow, deltaColumn, deltaCount);
}

void generateKnightChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns) {
  int deltaRow[]    = {-1,  1, 2,  2}; // the other moves follow by symmetry
  int deltaColumn[] = { 2,  2, 1, -1};
  ASSERT(sizeof(deltaRow) == sizeof(deltaColumn));

  size_t deltaCount = sizeof(deltaRow) / sizeof(int);

  generateChessIdeal(ideal, rowsAndColumns, rowsAndColumns,
					 deltaRow, deltaColumn, deltaCount);
}

bool generateRandomIdeal(BigIdeal& bigIdeal,
						 unsigned int exponentRange,
						 unsigned int variableCount,
						 unsigned int generatorCount) {
  Ideal ideal(variableCount);
  Term term(variableCount);

  unsigned int generatorsToGo = generatorCount;
  unsigned int triesLeft = (unsigned int)4 * 1000 * 1000;
  while (generatorsToGo > 0 && triesLeft > 0) {
    --triesLeft;

    for (unsigned int var = 0; var < variableCount; ++var) {
      term[var] = rand();
      if (exponentRange != numeric_limits<unsigned int>::max())
		term[var] %= exponentRange + 1;
    }

    if (ideal.isIncomparable(term)) {
      ideal.insert(term);
      --generatorsToGo;
    }

    --triesLeft;
  }

  VarNames names(variableCount);
  bigIdeal.clearAndSetNames(names);
  bigIdeal.insert(ideal);

  return generatorsToGo == 0;
}

void generateRandomFrobeniusInstance(vector<mpz_class>& degrees) {
  int numberCount = 10;//;4 + (rand() % 6);
  int mod = 100000;

  degrees.resize(numberCount);

  mpz_class totalGcd = 0;
  for (int i = 0; i < numberCount - 1; ++i) {
    mpz_class number = mpz_class(2+(rand() % mod));
    if (totalGcd == 0)
      totalGcd = number;
    else {
      mpz_gcd(totalGcd.get_mpz_t(),
			  totalGcd.get_mpz_t(),
			  number.get_mpz_t());
    }
    degrees[i] = number;
  }

  // This ensures that the gcd of all the numbers is 1.
  degrees[numberCount - 1] =
    (totalGcd == 1 ? mpz_class((rand() % mod) + 2) : totalGcd + 1);

  sort(degrees.begin(), degrees.end());
}
