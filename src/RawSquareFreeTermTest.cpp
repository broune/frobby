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
  ASSERT_EQ(getWordCount(0), 1u);
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
  const size_t maxVarCount = 2 * BitsPerWord + 1;

  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* term = newTerm(varCount);

	for (size_t var = 0; var < varCount; ++var) {
	  ASSERT_FALSE_SILENT(hasFullSupport(term, varCount));
	  setExponent(term, var, true);
	}
	ASSERT_TRUE_SILENT(hasFullSupport(term, varCount));

	for (size_t var = 0; var < varCount; ++var) {
	  ASSERT_TRUE_SILENT(hasFullSupport(term, varCount));
	  setExponent(term, var, false);
	  ASSERT_FALSE_SILENT(hasFullSupport(term, varCount));
	  setExponent(term, var, true);
	}
	ASSERT_TRUE(hasFullSupport(term, varCount));

	deleteTerm(term);
  }
}

TEST(RawSquareFreeTerm, IsIdentity) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
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

TEST(RawSquareFreeTerm, GetSizeOfSupport) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* term = newTerm(varCount);

	ASSERT_EQ(getSizeOfSupport(term, varCount), 0u);
	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(term, var, 1);
	  ASSERT_EQ_SILENT(getSizeOfSupport(term, varCount), 1u);
	  setExponent(term, var, 0);
	}

	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(term, var, 1);
	  ASSERT_EQ_SILENT(getSizeOfSupport(term, varCount), var + 1);
	}

	deleteTerm(term);
  }
}

TEST(RawSquareFreeTerm, SetToIdentity) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* term = newTerm(varCount);
	Word* termEnd = term + getWordCount(varCount);

	ASSERT_TRUE(isIdentity(term, termEnd));
	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(term, var, true);
	  setToIdentity(term, termEnd);
	  ASSERT_TRUE_SILENT(isIdentity(term, termEnd));

	  setExponent(term, var, true);
	  setToIdentity(term, varCount);
	  ASSERT_TRUE_SILENT(isIdentity(term, termEnd));
	}

	deleteTerm(term);
  }
}

TEST(RawSquareFreeTerm, SetToAllVarProd) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* term = newTerm(varCount);

	setToAllVarProd(term, varCount);
	ASSERT_TRUE(hasFullSupport(term, varCount));
	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(term, var, false);
	  setToAllVarProd(term, varCount);
	  ASSERT_TRUE_SILENT(isValid(term, varCount));
	  ASSERT_TRUE_SILENT(hasFullSupport(term, varCount));
	}

	deleteTerm(term);
  }
}

TEST(RawSquareFreeTerm, IsRelativelyPrime) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
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
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
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
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
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
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
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
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
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

TEST(RawSquareFreeTerm, LexLess) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);
	Word* b = newTerm(varCount);

	ASSERT_FALSE(lexLess(a, b, varCount));
	if (varCount == 0)
	  continue;
	setExponent(b, varCount - 1, 1);
	ASSERT_TRUE_SILENT(lexLess(a, b, varCount));
	ASSERT_FALSE_SILENT(lexLess(b, a, varCount));

	for (size_t var = 0; var < varCount - 1; ++var) {
	  setExponent(a, var, 1);
	  ASSERT_FALSE_SILENT(lexLess(a, b, varCount));
	  ASSERT_TRUE_SILENT(lexLess(b, a, varCount));

	  setExponent(b, var, 1);
	  ASSERT_TRUE_SILENT(lexLess(a, b, varCount));
	  ASSERT_FALSE_SILENT(lexLess(b, a, varCount));

	  if (var % 3 == 1) {
		// vary the pattern of the vars we've already been at.
		setExponent(a, var, 0);
		setExponent(b, var, 0);
	  }
	}

	deleteTerm(a);
	deleteTerm(b);
  }
}

TEST(RawSquareFreeTerm, Invert) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);

	ASSERT_TRUE(isIdentity(a, varCount));
	invert(a, varCount);
	ASSERT_TRUE(hasFullSupport(a, varCount));
	ASSERT_TRUE(isValid(a, varCount));

	if (varCount < 1)
	  continue;

	setExponent(a, 0, false);
	setExponent(a, varCount - 1, false);
	invert(a, varCount);
	ASSERT_TRUE(getExponent(a, 0));
	ASSERT_TRUE(getExponent(a, varCount - 1));

	invert(a, varCount);
	ASSERT_FALSE(getExponent(a, 0));
	ASSERT_FALSE(getExponent(a, varCount - 1));

	deleteTerm(a);
  }
}

TEST(RawSquareFreeTerm, GetVarIfPure) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);

	ASSERT_EQ(getVarIfPure(a, varCount), varCount);
	for (size_t v1 = 0; v1 < varCount; ++v1) {
	  setExponent(a, v1, 1);
	  ASSERT_EQ_SILENT(getVarIfPure(a, varCount), v1);
	  for (size_t v2 = 0; v2 < varCount; ++v2) {
		if (v1 != v2) {
		  setExponent(a, v2, 1);
		  ASSERT_EQ_SILENT(getVarIfPure(a, varCount), varCount);
		  setExponent(a, v2, 0);
		}
	  }
	  setExponent(a, v1, 0);
	}

	deleteTerm(a);
  }
}

/*
TEST(RawSquareFreeTerm, IncrementAtSupport) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* every = newTerm(varCount);
	Word* second = newTerm(varCount);
	Word* third = newTerm(varCount);

	vector<size_t> counts(varCount);
	vector<size_t> countsCorrect(varCount);
	size_t* countsPtr = &counts.front();

	incrementAtSupport(every, countsPtr, varCount);
	ASSERT_TRUE_SILENT(counts == vector<size_t>(varCount));

	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(every, var, 1);
	  countsCorrect[var] += 1;
	  if (var % 2 == 0) {
		setExponent(second, var, 1);
		countsCorrect[var] += 1;
	  }
	  if (var % 3 == 0) {
		setExponent(third, var, 1);
		countsCorrect[var] += 1;
	  }
	}

	incrementAtSupport(every, countsPtr, varCount);
	incrementAtSupport(second, countsPtr, varCount);
	incrementAtSupport(third, countsPtr, varCount);

	ASSERT_TRUE(counts == countsCorrect);

	deleteTerm(every);
	deleteTerm(second);
	deleteTerm(third);
  }
}
*/
TEST(RawSquareFreeTerm, IsValid) {
  const size_t varCount = 2 * BitsPerWord;
  Word* a = newTerm(varCount);
  ASSERT_TRUE(isValid(a, varCount));

  setExponent(a, 1, true);
  ASSERT_FALSE(isValid(a, 1));
  ASSERT_TRUE(isValid(a, 2));

  setExponent(a, BitsPerWord, true);
  ASSERT_TRUE(isValid(a, BitsPerWord));
  ASSERT_TRUE(isValid(a, BitsPerWord + 1));

  setExponent(a, BitsPerWord + 1, true);
  ASSERT_FALSE(isValid(a, BitsPerWord + 1));
  ASSERT_TRUE(isValid(a, BitsPerWord + 2));

  setExponent(a, varCount - 1, true);
  ASSERT_FALSE(isValid(a, varCount - 1));
  ASSERT_TRUE(isValid(a, varCount));

  deleteTerm(a);
}

TEST(RawSquareFreeTerm, NewTermParse) {
  Word* ref = newTerm(65);
  setExponent(ref, 0, 1);
  setExponent(ref, 1, 1);
  setExponent(ref, 3, 1);
  setExponent(ref, 4, 1);
  setExponent(ref, 64, 1);

  //  0        1         2         3         4         5         6        7
  //  1234567890123456789012345678901234567890123456789012345678901234567890
  Word* parsed = newTermParse
	("11011000000000000000000000000000000000000000000000000000000000001");
  ASSERT_TRUE(equals(ref, parsed, 65));
  deleteTerm(parsed);
  deleteTerm(ref);

  ref = newTerm(1);
  setExponent(ref, 0, 1);
  parsed = newTermParse("1");
  ASSERT_TRUE(equals(ref, parsed, 1));
  deleteTerm(parsed);

  deleteTerm(ref);
}

TEST(RawSquareFreeTerm, Equals) {
  const size_t maxVarCount = 2 * BitsPerWord + 1;
  for (size_t varCount = 0; varCount <= maxVarCount; ++varCount) {
	Word* a = newTerm(varCount);
	Word* b = newTerm(varCount);

	ASSERT_TRUE(equals(a, b, varCount));
	for (size_t var = 0; var < varCount; ++var) {
	  setExponent(a, var, 1);
	  ASSERT_FALSE_SILENT(equals(a, b, varCount));
	  ASSERT_FALSE_SILENT(equals(b, a, varCount));

	  setExponent(b, var, 1);
	  ASSERT_TRUE_SILENT(equals(a, b, varCount));
	  ASSERT_TRUE_SILENT(equals(b, a, varCount));
	}

	deleteTerm(a);
	deleteTerm(b);
  }
}

#define TEST_COMPACT(A,B,C) {					\
	size_t varCount = strlen(A);				\
	Word* a = newTermParse(A);					\
	Word* b = newTermParse(B);					\
	Word* c = newTermParse(C);					\
	size_t varCountAfter = strlen(C);			\
	Word* d = newTerm(varCountAfter);			\
	compact(d, a, b, varCount);					\
	ASSERT_TRUE(equals(d, c, varCountAfter));	\
	compact(a, a, b, varCount);					\
	ASSERT_TRUE(equals(a, c, varCountAfter));	\
	deleteTerm(a);								\
	deleteTerm(b);								\
	deleteTerm(c);								\
	deleteTerm(d);								\
  }
TEST(RawSquareFreeTerm, Compact) {
  TEST_COMPACT("0", "0", "0");
  TEST_COMPACT("1", "0", "1");
  TEST_COMPACT("0", "1", "");
  TEST_COMPACT("1", "1", "");
  TEST_COMPACT("001", "010", "01");
  TEST_COMPACT("000000000000000000001111111111111111111111111111",
			   "000000000000000000000000000000000000000000000000",
			   "000000000000000000001111111111111111111111111111");
  TEST_COMPACT("000000000000000000001111111111111111111111111111",
			   "111111111111111111111111111111111111111111111111",
			   "");
  TEST_COMPACT("111100001111000011110000111100001111000011110000",
			   "101010101010101010101010101010101010101010101010",
			   "110011001100110011001100");
  TEST_COMPACT("000000000000000000000000000000100000000000000001",
			   "011111111111111111111111111111011111111111111101",
			   "010");
  TEST_COMPACT("011111111111111111111111111111011111111111111101",
			   "000000000000000000000000000000100000000000000001",
			   "0111111111111111111111111111111111111111111110");
}
