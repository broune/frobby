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
#include <ctime>

void generateLinkedListIdeal(BigIdeal& ideal, size_t variableCount) {
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
                        size_t rowCount,
                        size_t columnCount,
                        int deltaRow[],
                        int deltaColumn[],
                        size_t deltaCount) {
  if (mpz_class(rowCount) * mpz_class(columnCount) >
      numeric_limits<size_t>::max())
    reportError("Number of positions on requested chess board too large.");

  // Generate names
  VarNames names;
  for (size_t row = 0; row < rowCount; ++row) {
    for (size_t column = 0; column < columnCount; ++column) {
      FrobbyStringStream name;
      name << 'r' << (row + 1) << 'c' << (column + 1);
      names.addVar(name);
    }
  }
  bigIdeal.clearAndSetNames(names);
  Ideal ideal(bigIdeal.getVarCount());

  // Generate ideal
  for (size_t row = 0; row < rowCount; ++row) {
    for (size_t column = 0; column < columnCount; ++column) {
      for (size_t delta = 0; delta < deltaCount; ++delta) {
        // Check that the target position is within the board.

        if (deltaRow[delta] == numeric_limits<int>::min() ||
            (deltaRow[delta] < 0 &&
             row < (size_t)-deltaRow[delta]) ||
            (deltaRow[delta] > 0 &&
             rowCount - row <= (size_t)deltaRow[delta]))
          continue;

        if (deltaColumn[delta] == numeric_limits<int>::min() ||
            (deltaColumn[delta] < 0 &&
             column < (size_t)-deltaColumn[delta]) ||
            (deltaColumn[delta] > 0 &&
             columnCount - column <= (size_t)deltaColumn[delta]))
          continue;

        Term chessMove(ideal.getVarCount());
        chessMove[row * columnCount + column] = 1;

        size_t targetRow = row + deltaRow[delta];
        size_t targetColumn = column + deltaColumn[delta];
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

void generateKingChessIdeal(BigIdeal& ideal, size_t rowsAndColumns) {
  int deltaRow[]    = {-1, 0, 1, 1}; // the other moves follow by symmetry
  int deltaColumn[] = { 1, 1, 1, 0};
  ASSERT(sizeof(deltaRow) == sizeof(deltaColumn));

  size_t deltaCount = sizeof(deltaRow) / sizeof(int);

  generateChessIdeal(ideal, rowsAndColumns, rowsAndColumns,
                     deltaRow, deltaColumn, deltaCount);
}

void generateKnightChessIdeal(BigIdeal& ideal, size_t rowsAndColumns) {
  int deltaRow[]    = {-1,  1, 2,  2}; // the other moves follow by symmetry
  int deltaColumn[] = { 2,  2, 1, -1};
  ASSERT(sizeof(deltaRow) == sizeof(deltaColumn));

  size_t deltaCount = sizeof(deltaRow) / sizeof(int);

  generateChessIdeal(ideal, rowsAndColumns, rowsAndColumns,
                     deltaRow, deltaColumn, deltaCount);
}

namespace {
  /** Consider the entries of pattern as the bits in a binary
   number. This method adds 1 and returns true if the resulting
   pattern is all zeroes. */
  bool nextBitPattern(vector<char>& pattern) {
    typedef vector<char>::iterator iterator;
    for (iterator it = pattern.begin(); it != pattern.end(); ++it) {
      if (*it)
        *it = 0;
      else {
        *it = 1;
        ASSERT(pattern != vector<char>(pattern.size()));
        return true;
      }
    }

    ASSERT(pattern == vector<char>(pattern.size()));
    return false;
  }
}

void generateTreeIdeal(BigIdeal& ideal, size_t varCount) {
  ideal.clearAndSetNames(VarNames(varCount));

  // Declare outside of loop to avoid repeated initialization.
  mpz_class exponent;

  // Using vector<char> to avoid vector<bool> which has special
  // properties. Going through all "bit" patterns by simulating adding
  // one at each step. pattern starts at all zero, which represents
  // the identity, so we take the next bit pattern even in the first
  // iteration to go past that.
  vector<char> pattern(varCount);
  while (nextBitPattern(pattern)) {
    size_t setSize = 0;
    typedef vector<char>::iterator iterator;
    for (iterator it = pattern.begin(); it != pattern.end(); ++it)
      setSize += (size_t)*it;

    exponent = varCount - setSize + 1;
    ideal.newLastTerm();
    for (size_t var = 0; var < varCount; ++var)
      if (pattern[var])
        ideal.getLastTermExponentRef(var) = exponent;
  }
}

bool generateRandomEdgeIdeal
(BigIdeal& bigIdeal, size_t variableCount, size_t generatorCount) {
  Ideal ideal(variableCount);
  Term term(variableCount);

  size_t generatorsToGo = generatorCount;
  size_t triesLeft = (size_t)4 * 1000 * 1000;
  while (generatorsToGo > 0 && triesLeft > 0) {
    --triesLeft;

    size_t a = rand() % variableCount;
    size_t b = rand() % variableCount;
    if (a == b)
      continue;

    term[a] = 1;
    term[b] = 1;

    if (ideal.isIncomparable(term)) {
      ideal.insert(term);
      --generatorsToGo;
    }

    term[a] = 0;
    term[b] = 0;

    --triesLeft;
  }

  VarNames names(variableCount);
  bigIdeal.clearAndSetNames(names);
  bigIdeal.insert(ideal);

  return generatorsToGo == 0;
}


bool generateRandomIdeal(BigIdeal& bigIdeal,
                         size_t exponentRange,
                         size_t variableCount,
                         size_t generatorCount) {
  Ideal ideal(variableCount);
  Term term(variableCount);

  size_t generatorsToGo = generatorCount;
  size_t triesLeft = (size_t)4 * 1000 * 1000;
  while (generatorsToGo > 0 && triesLeft > 0) {
    --triesLeft;

    for (size_t var = 0; var < variableCount; ++var) {
      term[var] = rand();
      if (exponentRange != numeric_limits<size_t>::max())
        term[var] %= exponentRange + 1;
    }

    if (ideal.isIncomparable(term)) {
      ideal.insert(term);
      --generatorsToGo;
    }
  }

  VarNames names(variableCount);
  bigIdeal.clearAndSetNames(names);
  bigIdeal.insert(ideal);

  return generatorsToGo == 0;
}

void generateRandomFrobeniusInstance(vector<mpz_class>& instance,
                                     size_t entryCount,
                                     const mpz_class& maxEntry) {
  ASSERT(entryCount >= 1);
  ASSERT(maxEntry >= 1);

  gmp_randclass random(gmp_randinit_default);

  /// @todo: preserve state across calls.
  random.seed((unsigned long)time(0) +
#ifdef __GNUC__ // Only GCC defines this macro.
              (unsigned long)getpid() +
#endif
              (unsigned long)clock());

  instance.resize(entryCount);

  // Populate instance with random numbers in range [1,maxEntry].
  for (size_t i = 0; i < entryCount; ++i)
    instance[i] = random.get_z_range(maxEntry) + 1;

  // Calculate greatest common divisor of instance.
  mpz_class gcd = instance[0];
  for (size_t i = 1; i < entryCount; ++i)
    mpz_gcd(gcd.get_mpz_t(), gcd.get_mpz_t(), instance[i].get_mpz_t());

  // Ensure that instance are relatively prime.
  instance.front() /= gcd;

  sort(instance.begin(), instance.end());
}
