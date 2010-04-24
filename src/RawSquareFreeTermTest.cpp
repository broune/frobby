/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "RawSquareFreeTerm.h"
#include "tests.h"

TEST_SUITE(RawSquareFreeTerm)

using namespace SquareFreeTermOps;
namespace Ops = SquareFreeTermOps;

TEST(RawSquareFreeTerm, getWordCount) {
  ASSERT_EQ(getWordCount(0), 0u);
  ASSERT_EQ(getWordCount(1), 1u);

  ASSERT_EQ(getWordCount(BitsPerWord - 1), 1u);
  ASSERT_EQ(getWordCount(BitsPerWord), 1u);
  ASSERT_EQ(getWordCount(BitsPerWord + 1), 2u);

  ASSERT_EQ(getWordCount(10 * BitsPerWord - 1), 10u);
  ASSERT_EQ(getWordCount(10 * BitsPerWord), 10u);
  ASSERT_EQ(getWordCount(10 * BitsPerWord + 1), 11u);
}

TEST(RawSquareFreeTerm, SetAndGetExponent) {
  const size_t varCount = BitsPerWord * 2;
  Word* term = newTerm(varCount);

  for (size_t var = 0; var < varCount; ++var) {
	ASSERT_FALSE(getExponent(term, var));
	setExponent(term, var, true);
	ASSERT_TRUE(getExponent(term, var));
  }

  for (size_t var = 0; var < varCount; ++var) {
	ASSERT_TRUE(getExponent(term, var));
	setExponent(term, var, false);
	ASSERT_FALSE(getExponent(term, var));
  }
}

TEST(RawSquareFreeTerm, Assign) {
  const size_t varCount = BitsPerWord * 2;
  const size_t wordCount = getWordCount(varCount);
  Word* a = newTerm(varCount);
  Word* b = newTerm(varCount);

  setExponent(a, 1, true);

  ASSERT_TRUE(getExponent(a, 1));
  ASSERT_FALSE(getExponent(b, 1));

  assign(b, b + wordCount, a);

  ASSERT_TRUE(getExponent(a, 1));
  ASSERT_TRUE(getExponent(b, 1));

  setExponent(a, 1, false);

  ASSERT_FALSE(getExponent(a, 1));
  ASSERT_TRUE(getExponent(b, 1));

  deleteTerm(a);
  deleteTerm(b);
}

TEST(RawSquareFreeTerm, HasFullSupport) {
  const size_t maxVarCount = 2 * BitsPerWord;

  for (size_t varCount = 0; varCount < maxVarCount; ++varCount) {
	Word* term = newTerm(varCount);
	Word* termEnd = term + getWordCount(varCount);

	for (size_t var = 0; var < varCount; ++var) {
	  ASSERT_FALSE_SILENT(hasFullSupport(term, termEnd, varCount));
	  setExponent(term, var, true);
	}
	ASSERT_TRUE_SILENT(hasFullSupport(term, termEnd, varCount));

	for (size_t var = 0; var < varCount; ++var) {
	  ASSERT_TRUE_SILENT(hasFullSupport(term, termEnd, varCount));
	  setExponent(term, var, false);
	  ASSERT_FALSE_SILENT(hasFullSupport(term, termEnd, varCount));
	  setExponent(term, var, true);
	}
	ASSERT_TRUE(hasFullSupport(term, termEnd, varCount));

	deleteTerm(term);
  }  
}

TEST(RawSquareFreeTerm, IsIdentity) {
  const size_t maxVarCount = 2 * BitsPerWord;
  for (size_t varCount = 0; varCount < maxVarCount; ++varCount) {
	Word* term = newTerm(varCount);
	Word* termEnd = term + getWordCount(varCount);

	ASSERT_TRUE(isIdentity(term, termEnd));
	for (size_t var = 0; var < varCount; ++var) {
	  ASSERT_TRUE_SILENT(isIdentity(term, termEnd));
	  setExponent(term, var, true);
	  ASSERT_FALSE_SILENT(isIdentity(term, termEnd));
	  setExponent(term, var, false);
	}
	ASSERT_TRUE(isIdentity(term, termEnd));

	deleteTerm(term);
  }  
}

TEST(RawSquareFreeTerm, SetToIdentity) {
  const size_t maxVarCount = 2 * BitsPerWord;
  for (size_t varCount = 0; varCount < maxVarCount; ++varCount) {
	Word* term = newTerm(varCount);
	Word* termEnd = term + getWordCount(varCount);

	ASSERT_TRUE(isIdentity(term, termEnd));
	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(term, var, true);
	  setToIdentity(term, termEnd);
	  ASSERT_TRUE_SILENT(isIdentity(term, termEnd));
	}

	deleteTerm(term);
  }  
}

TEST(RawSquareFreeTerm, IsRelativelyPrime) {
  const size_t maxVarCount = 2 * BitsPerWord;
  for (size_t varCount = 0; varCount < maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);
	Word* aEnd = a + getWordCount(varCount);
	Word* b = newTerm(varCount);
	Word* bEnd = b + getWordCount(varCount);

	ASSERT_TRUE(isRelativelyPrime(a, aEnd, b));
	ASSERT_TRUE(isRelativelyPrime(b, bEnd, a));
	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(a, var, true);
	  ASSERT_TRUE_SILENT(isRelativelyPrime(a, aEnd, b));
	  ASSERT_TRUE_SILENT(isRelativelyPrime(b, bEnd, a));
	  setExponent(b, var, true);
	  ASSERT_FALSE_SILENT(isRelativelyPrime(a, aEnd, b));
	  ASSERT_FALSE_SILENT(isRelativelyPrime(b, bEnd, a));
	  setExponent(a, var, false);
	}

	deleteTerm(a);
	deleteTerm(b);
  }
}

TEST(RawSquareFreeTerm, Lcm) {
  const size_t maxVarCount = 2 * BitsPerWord;
  for (size_t varCount = 0; varCount < maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);
	Word* b = newTerm(varCount);
	Word* c = newTerm(varCount);
	Word* cEnd = c + getWordCount(varCount);

	lcm(c, cEnd, a, b);
	ASSERT_TRUE(isIdentity(c, cEnd));
	if (varCount < 4)
	  continue;

	// a becomes 1001 at end.
	// b becomes 1100 at end.
	setExponent(a, varCount - 1, true);
	setExponent(a, varCount - 4, true);
	setExponent(b, varCount - 3, true);
	setExponent(b, varCount - 4, true);
	lcm(c, cEnd, a, b);

	// c should be 1101 at end.
	ASSERT_TRUE(getExponent(c, varCount - 1));
	ASSERT_FALSE(getExponent(c, varCount - 2));
	ASSERT_TRUE(getExponent(c, varCount - 3));
	ASSERT_TRUE(getExponent(c, varCount - 4));

	// no other bits should be set.
	setExponent(c, varCount - 1, false);
	setExponent(c, varCount - 3, false);
	setExponent(c, varCount - 4, false);
	ASSERT_TRUE(isIdentity(c, cEnd));

	deleteTerm(a);
	deleteTerm(b);
	deleteTerm(c);
  }
}

TEST(RawSquareFreeTerm, Gcd) {
  const size_t maxVarCount = 2 * BitsPerWord;
  for (size_t varCount = 0; varCount < maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);
	Word* b = newTerm(varCount);
	Word* c = newTerm(varCount);
	Word* cEnd = c + getWordCount(varCount);

	gcd(c, cEnd, a, b);
	ASSERT_TRUE(isIdentity(c, cEnd));
	if (varCount < 4)
	  continue;

	// a becomes 1001 at end.
	// b becomes 1100 at end.
	setExponent(a, varCount - 1, true);
	setExponent(a, varCount - 4, true);
	setExponent(b, varCount - 3, true);
	setExponent(b, varCount - 4, true);
	gcd(c, cEnd, a, b);

	// c should be 1000 at end.
	ASSERT_FALSE(getExponent(c, varCount - 1));
	ASSERT_FALSE(getExponent(c, varCount - 2));
	ASSERT_FALSE(getExponent(c, varCount - 3));
	ASSERT_TRUE(getExponent(c, varCount - 4));

	// no other bits should be set.
	setExponent(c, varCount - 4, false);
	ASSERT_TRUE(isIdentity(c, cEnd));

	deleteTerm(a);
	deleteTerm(b);
	deleteTerm(c);
  }
}

TEST(RawSquareFreeTerm, Colon) {
  const size_t maxVarCount = 2 * BitsPerWord;
  for (size_t varCount = 0; varCount < maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);
	Word* b = newTerm(varCount);
	Word* c = newTerm(varCount);
	Word* cEnd = c + getWordCount(varCount);

	colon(c, cEnd, a, b);
	ASSERT_TRUE(isIdentity(c, cEnd));
	if (varCount < 4)
	  continue;

	// a becomes 1001 at end.
	// b becomes 1100 at end.
	setExponent(a, varCount - 1, true);
	setExponent(a, varCount - 4, true);
	setExponent(b, varCount - 3, true);
	setExponent(b, varCount - 4, true);
	colon(c, cEnd, a, b);

	// c should be 0001 at end.
	ASSERT_TRUE(getExponent(c, varCount - 1));
	ASSERT_FALSE(getExponent(c, varCount - 2));
	ASSERT_FALSE(getExponent(c, varCount - 3));
	ASSERT_FALSE(getExponent(c, varCount - 4));

	// no other bits should be set.
	setExponent(c, varCount - 1, false);
	ASSERT_TRUE(isIdentity(c, cEnd));

	deleteTerm(a);
	deleteTerm(b);
	deleteTerm(c);
  }
}

TEST(RawSquareFreeTerm, Divides) {
  const size_t maxVarCount = 2 * BitsPerWord;
  for (size_t varCount = 0; varCount < maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);
	Word* aEnd = a + getWordCount(varCount);
	Word* b = newTerm(varCount);
	Word* bEnd = b + getWordCount(varCount);

	ASSERT_TRUE(Ops::divides(a, aEnd, b));
	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(a, var, true);
	  ASSERT_FALSE_SILENT(Ops::divides(a, aEnd, b));
	  ASSERT_TRUE_SILENT(Ops::divides(b, bEnd, a));
	  setExponent(b, var, true);
	  ASSERT_TRUE_SILENT(Ops::divides(a, aEnd, b));
	}

	deleteTerm(a);
	deleteTerm(b);
  }
}
