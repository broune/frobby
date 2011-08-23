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
#include "RawSquareFreeIdeal.h"
#include "tests.h"

#include "RawSquareFreeTerm.h"

using namespace SquareFreeTermOps;

typedef RawSquareFreeIdeal RSFIdeal;

TEST_SUITE(RawSquareFreeIdeal)

namespace {
  bool sortedEqual(const RawSquareFreeIdeal& a, const RawSquareFreeIdeal& b) {
	RSFIdeal* aCopy = newRawSquareFreeIdeal(a);
	RSFIdeal* bCopy = newRawSquareFreeIdeal(b);
	aCopy->sortLexAscending();
	bCopy->sortLexAscending();
	bool equal = (*aCopy == *bCopy);
	deleteRawSquareFreeIdeal(aCopy);
	deleteRawSquareFreeIdeal(bCopy);
	return equal;
  }
}

TEST(RawSquareFreeIdeal, Insert_Term) {
  const size_t varCount = 5;
  Word* a = newTermParse("11111");
  Word* b = newTermParse("00000");
  Word* c = newTermParse("10101");

  RSFIdeal* ideal = newRawSquareFreeIdeal(5, 3);
  ASSERT_TRUE(ideal->getGeneratorCount() == 0);
  ideal->insert(a);
  ideal->insert(b);
  ideal->insert(c);

  ASSERT_TRUE(ideal->getGeneratorCount() == 3);
  ASSERT_TRUE(equals(ideal->getGenerator(0), a, varCount));
  ASSERT_TRUE(equals(ideal->getGenerator(1), b, varCount));
  ASSERT_TRUE(equals(ideal->getGenerator(2), c, varCount));

  deleteRawSquareFreeIdeal(ideal);
  deleteTerm(a);
  deleteTerm(b);
  deleteTerm(c);
}

TEST(RawSquareFreeIdeal, NewIdealParse) {
  const size_t varCount = 5;
  Word* a = newTermParse("11111");
  Word* b = newTermParse("00000");

  RSFIdeal* ideal = newRawSquareFreeIdealParse("11111\n00000\n");
  ASSERT_TRUE(ideal->getGeneratorCount() == 2);
  ASSERT_TRUE(equals(ideal->getGenerator(0), a, varCount));
  ASSERT_TRUE(equals(ideal->getGenerator(1), b, varCount));

  deleteRawSquareFreeIdeal(ideal);
  deleteTerm(a);
  deleteTerm(b);
}

TEST(RawSquareFreeIdeal, Equality) {
  vector<RawSquareFreeIdeal*> ideals;
  ideals.push_back
	(newRawSquareFreeIdealParse
	 ("000\n"
	  "001\n"
	  "010\n"
	  "011\n"
	  "100\n"
	  "101\n"
	  "110\n"
	  "111\n"));
  ideals.push_back
	(newRawSquareFreeIdealParse
	 ("001\n"
	  "000\n"
	  "010\n"
	  "011\n"
	  "100\n"
	  "101\n"
	  "110\n"
	  "111\n"));
  ideals.push_back(newRawSquareFreeIdealParse("1"));
  ideals.push_back(newRawSquareFreeIdealParse("0"));
  ideals.push_back(newRawSquareFreeIdealParse(""));

  for (size_t i = 0; i < ideals.size(); ++i) {
	for (size_t j = 0; j < ideals.size(); ++j) {
	  if (i == j)
		ASSERT_EQ(*ideals[i], *ideals[j]);
	  else
		ASSERT_NEQ(*ideals[i], *ideals[j]);
	}
  }

  for (size_t i = 0; i < ideals.size(); ++i)
	deleteRawSquareFreeIdeal(ideals[i]);
}

TEST(RawSquareFreeIdeal, SortLexAscending) {
  RSFIdeal* sorted = newRawSquareFreeIdealParse
	("000\n"
	 "001\n"
	 "010\n"
	 "011\n"
	 "100\n"
	 "101\n"
	 "110\n"
	 "111\n");
  RSFIdeal* shuffled = newRawSquareFreeIdealParse
	("111\n"
	 "000\n"
	 "101\n"
	 "011\n"
	 "100\n"
	 "001\n"
	 "110\n"
	 "010\n");
  shuffled->sortLexAscending();
  ASSERT_EQ(*sorted, *shuffled);

  deleteRawSquareFreeIdeal(sorted);
  deleteRawSquareFreeIdeal(shuffled);
}

#define TEST_MINIMIZE(idealStr, minimizedStr) {						\
  RSFIdeal* ideal = newRawSquareFreeIdealParse(idealStr);			\
  RSFIdeal* minimized = newRawSquareFreeIdealParse(minimizedStr);	\
  ASSERT_FALSE(ideal->isMinimallyGenerated());						\
  ideal->minimize();												\
  ASSERT_TRUE(sortedEqual(*ideal, *minimized));						\
  ASSERT_TRUE(ideal->isMinimallyGenerated());						\
  ideal->minimize();												\
  ASSERT_TRUE(sortedEqual(*ideal, *minimized));						\
  deleteRawSquareFreeIdeal(ideal);									\
  deleteRawSquareFreeIdeal(minimized);								\
  }

TEST(RawSquareFreeIdeal, MinimizeAndMinimizable) {
  TEST_MINIMIZE("000\n000\n000\n001\n001\n001\n001", "000");
  TEST_MINIMIZE("111\n111\n111\n111\n111\n111", "111");
  TEST_MINIMIZE("0\n1", "0");
  TEST_MINIMIZE("1\n0", "0");
  TEST_MINIMIZE
	("111111111111111111110000000000000000000000011111111111111111111111101\n"
	 "111111111111111111111111111111111111111111111111111111111111111111111\n"
	 "000000000000000000000000000000000000000000000000000000000000000000010\n",
	 "111111111111111111110000000000000000000000011111111111111111111111101\n"
	 "000000000000000000000000000000000000000000000000000000000000000000010\n");
  TEST_MINIMIZE
	("1001\n"
	 "1010\n"
	 "1100\n"
	 "0101\n"
	 "0100\n"
	 "0001\n"
	 "0110\n"
	 "0011\n",
	 "1010\n"
	 "0100\n"
	 "0001\n");
}

#define TEST_COLON_REMINIMIZE_TERM(idealStr, colonStr, minimizedStr) {	\
	RSFIdeal* ideal = newRawSquareFreeIdealParse(idealStr);				\
	Word* colon = newTermParse(colonStr);								\
	RSFIdeal* minimized = newRawSquareFreeIdealParse(minimizedStr);		\
	ideal->colonReminimize(colon);										\
	ASSERT_TRUE2(sortedEqual(*ideal, *minimized), *ideal, *minimized);	\
	ASSERT_TRUE(ideal->isMinimallyGenerated());							\
	deleteRawSquareFreeIdeal(ideal);									\
	deleteTerm(colon);													\
	deleteRawSquareFreeIdeal(minimized);								\
  }

#define TEST_COLON_REMINIMIZE_VAR(idealStr, colonVar, minimizedStr) {	\
	RSFIdeal* idealVar = newRawSquareFreeIdealParse(idealStr);			\
	RSFIdeal* idealTerm = newRawSquareFreeIdealParse(idealStr);			\
	Word* colon = newTerm(idealTerm->getVarCount());					\
	setExponent(colon, colonVar, 1);									\
	RSFIdeal* minimized = newRawSquareFreeIdealParse(minimizedStr);		\
	idealVar->colonReminimize((size_t)colonVar);						\
	ASSERT_TRUE2(sortedEqual(*idealVar, *minimized), *idealVar, *minimized); \
	idealTerm->colonReminimize(colon);									\
	ASSERT_TRUE2(sortedEqual(*idealTerm, *minimized), *idealVar, *minimized); \
	deleteRawSquareFreeIdeal(idealVar);									\
	deleteRawSquareFreeIdeal(idealTerm);								\
	deleteTerm(colon);													\
	deleteRawSquareFreeIdeal(minimized);								\
  }

TEST(RawSquareFreeIdeal, ColonReminimizeMinimize_VarAndTerm) {
  TEST_COLON_REMINIMIZE_VAR("0", 0, "0");
  TEST_COLON_REMINIMIZE_VAR("1", 0, "0");
  TEST_COLON_REMINIMIZE_VAR("111", 1, "101");
  TEST_COLON_REMINIMIZE_VAR("101\n110", 1, "100");
  TEST_COLON_REMINIMIZE_VAR("110\n101\n011", 2, "100\n010");
  TEST_COLON_REMINIMIZE_VAR("1100\n1010\n0110", 3, "1100\n1010\n0110");
  TEST_COLON_REMINIMIZE_VAR("1101\n1011\n0111", 3, "1100\n1010\n0110");
  TEST_COLON_REMINIMIZE_VAR
	("011111111111111111110000000000000000000000011111111111111111111111101\n"
	 "011111111111111111111111111111111111111111111011111111111111111111101\n"
	 "000000000000000000000000000000000000000000000100000000000000000000010\n", 67,
	 "011111111111111111111111111111111111111111111011111111111111111111101\n"
	 "000000000000000000000000000000000000000000000100000000000000000000000\n");
  TEST_COLON_REMINIMIZE_VAR
	("100000000000000\n"
	 "010000000000000\n"
	 "001000000000000\n"
	 "000100000000000\n"
	 "000010000000000\n"
	 "000001000000000\n"
	 "000000100000000\n"
	 "000000010000000\n"
	 "000000001000000\n"
	 "000000000100000\n"
	 "000000000010000\n"
	 "000000000001000\n"
	 "000000000000100\n"
	 "000000000000010\n"
	 "000000000000001\n", 0,
	 "000000000000000\n");


  TEST_COLON_REMINIMIZE_TERM("", "", "");
  TEST_COLON_REMINIMIZE_TERM("0", "0", "0");
  TEST_COLON_REMINIMIZE_TERM("0", "1", "0");
  TEST_COLON_REMINIMIZE_TERM("1", "0", "1");
  TEST_COLON_REMINIMIZE_TERM("1", "1", "0");
  TEST_COLON_REMINIMIZE_TERM("111", "001", "110");
  TEST_COLON_REMINIMIZE_TERM("111", "110", "001");
  TEST_COLON_REMINIMIZE_TERM("1101\n1110", "1001", "0100");
  TEST_COLON_REMINIMIZE_TERM("1101\n1011\n0110", "0011", "1000\n0100");
  TEST_COLON_REMINIMIZE_TERM("11000\n10100\n01100", "00011",
							 "11000\n10100\n01100");
  TEST_COLON_REMINIMIZE_TERM("11011\n10111\n01111", "00011",
							 "11000\n10100\n01100");
  TEST_COLON_REMINIMIZE_TERM
	("011111111111111111110000000000000000000000011111111111111111111111101\n"
	 "011111111111111111111111111111111111111111111011111111111111111111101\n"
	 "100000000000000000000000000000000000000000000100000000000000000000010\n",
	 "100000000000000000000000000000000000000000000000000000100000000000010",
	 "011111111111111111111111111111111111111111111011111111011111111111101\n"
	 "000000000000000000000000000000000000000000000100000000000000000000000\n");
  TEST_COLON_REMINIMIZE_TERM
	("100000000000000\n"
	 "010000000000000\n"
	 "001000000000000\n"
	 "000100000000000\n"
	 "000010000000000\n"
	 "000001000000000\n"
	 "000000100000000\n"
	 "000000010000000\n"
	 "000000001000000\n"
	 "000000000100000\n"
	 "000000000010000\n"
	 "000000000001000\n"
	 "000000000000100\n"
	 "000000000000010\n"
	 "000000000000001\n",
	 "100000000000001",
	 "000000000000000\n");
}

TEST(RawSquareFreeIdeal, GetVarDividesCounts) {
  const size_t varCount = 2 * BitsPerWord + 1;
  RSFIdeal* ideal = newRawSquareFreeIdeal(varCount, varCount + 33);
  Word* term = newTerm(varCount);
  vector<size_t> countsCorrect(varCount);
  vector<size_t> counts;

  ideal->getVarDividesCounts(counts);
  ASSERT_EQ(counts, countsCorrect);

  setToAllVarProd(term, varCount);
  const size_t insertAllOnesCount = varCount < 33 ? varCount : 33;
  for (size_t i = 0; i < insertAllOnesCount; ++i) {
	ideal->insert(term);
	for (size_t var = 0; var < varCount; ++var)
	  countsCorrect[var] += 1;
	ideal->getVarDividesCounts(counts);
	ASSERT_EQ(counts, countsCorrect);
  }

  ASSERT(ideal->getGeneratorCount() <= varCount);
  setToIdentity(term, varCount);
  for (size_t i = 0; i < varCount; ++i) {
    setExponent(ideal->getGenerator(i % insertAllOnesCount), i, 0);
	ideal->insert(term);
	countsCorrect[i] -= 1;
	ideal->getVarDividesCounts(counts);
	ASSERT_EQ(counts, countsCorrect);
  }

  deleteRawSquareFreeIdeal(ideal);
  deleteTerm(term);
}

#define TEST_HASFULLSUPPORT(idealStr, _extraStr, value) {				\
	const char* extraStr = _extraStr;									\
	Word* extra = extraStr == 0 ? 0 : newTermParse(extraStr);			\
	RSFIdeal* ideal = newRawSquareFreeIdealParse(idealStr);				\
	if (value) {														\
	  ASSERT_TRUE2(ideal->hasFullSupport(extra), *ideal, extraStr);		\
	} else {															\
	  ASSERT_FALSE2(ideal->hasFullSupport(extra), *ideal, extraStr);	\
	}																	\
	deleteRawSquareFreeIdeal(ideal);									\
	deleteTerm(extra);													\
  }
TEST(RawSquareFreeIdeal, HasFullSupport) {
  TEST_HASFULLSUPPORT("", "", true);
  TEST_HASFULLSUPPORT("0", "0", false);

  TEST_HASFULLSUPPORT("0\n0", "0", false);
  TEST_HASFULLSUPPORT("1\n0", "0", true);
  TEST_HASFULLSUPPORT("0\n1", "0", true);
  TEST_HASFULLSUPPORT("1\n1", "0", true);

  TEST_HASFULLSUPPORT("0\n0", "1", true);
  TEST_HASFULLSUPPORT("1\n0", "1", true);
  TEST_HASFULLSUPPORT("0\n1", "1", true);
  TEST_HASFULLSUPPORT("1\n1", "1", true);

  TEST_HASFULLSUPPORT
	("011111111111111111110000000000000000000000011011111111111111111111101\n"
	 "111111111111111111111111111111111111111111111011111111111111111111101\n"
	 "000000000000000000000000000000000000000000000000000000000000000000010\n",
	 "000000000000000000000000000000000000000000000100000000000000000000000",
	 true);

  TEST_HASFULLSUPPORT
	("011111111111111111110000000000000000000000011011111111111111111111101\n"
	 "111111111111111111111111111111111111111111111011111111111111111111101\n"
	 "000000000000000000000000000000000000000000000000000000000000000000010\n",
	 "000000000000000000000000000000000000000000000000000000000000000000000",
	 false);
  TEST_HASFULLSUPPORT
	("011111111111111111110000000000000000000000011011111111111111111111101\n"
	 "111111111111111111111111111111111111111111111011111111111111111111101\n"
	 "000000000000000000000000000000000000000000000000000000000000000000000\n",
	 "000000000000000000000000000000000000000000000100000000000000000000000",
	 false);

  TEST_HASFULLSUPPORT // this triggered a bug
	("11111111111111111111111111111111\n",
	 "00000000000000000000000000000000",
	 true);
}

#define TEST_COMPACT(beforeStr, removeStr, afterStr) {			\
	RSFIdeal* before = newRawSquareFreeIdealParse(beforeStr);	\
	Word* remove = newTermParse(removeStr);						\
	RSFIdeal* after = newRawSquareFreeIdealParse(afterStr);		\
    before->compact(remove);									\
	ASSERT_EQ(*before, *after);									\
	deleteRawSquareFreeIdeal(before);							\
	deleteTerm(remove);											\
	deleteRawSquareFreeIdeal(after);							\
  }
TEST(RawSquareFreeIdeal, Compact) {
  TEST_COMPACT("101", "110", "1");
  TEST_COMPACT("101", "101", "0");
  TEST_COMPACT("101", "000", "101");
  TEST_COMPACT("111\n000\n001\n101", "110", "1\n0\n1\n1\n");

  TEST_COMPACT
	("011111111111111111110000000000000000000000011011111111111111111111101\n"
	 "111111111111111111111111111111111111111111111011111111111111111111101\n"
	 "000000000000000000000000010000000000000000000000000000000000000000010\n",
	 "111111110000000000000000000000000000000000000000000000000000000000001",
	 "111111111111000000000000000000000001101111111111111111111110\n"
	 "111111111111111111111111111111111111101111111111111111111110\n"
	 "000000000000000001000000000000000000000000000000000000000001\n");
}

#define TEST_TRANSPOSE(beforeStr, removeStr, afterStr) {	  \
  RSFIdeal* before = newRawSquareFreeIdealParse(beforeStr);   \
  Word* remove = removeStr == 0 ? 0 : newTermParse(removeStr);\
  RSFIdeal* after = newRawSquareFreeIdealParse(afterStr);     \
  const size_t maxDim = before->getGeneratorCount() > before->getVarCount() ?\
    before->getGeneratorCount() : before->getVarCount();      \
  RSFIdeal* calculated = newRawSquareFreeIdeal(maxDim, maxDim);\
  calculated->setToTransposeOf(*before, remove);              \
  ASSERT_EQ(*calculated, *after);                             \
  calculated->setToTransposeOf(*calculated);                  \
  calculated->transpose();                                    \
  ASSERT_EQ(*calculated, *after);                             \
  deleteRawSquareFreeIdeal(before);							  \
  deleteTerm(remove);									      \
  deleteRawSquareFreeIdeal(after);							  \
  deleteRawSquareFreeIdeal(calculated);					      \
}

TEST(RawSquareFreeIdeal, Transpose) {
  TEST_TRANSPOSE("\n", 0, "\n");
  TEST_TRANSPOSE("\n", "", "\n");
  TEST_TRANSPOSE("01\n", 0, "0\n1\n");
  TEST_TRANSPOSE("01\n", "00", "0\n1\n");
  TEST_TRANSPOSE("01\n", "10", "1\n");
  TEST_TRANSPOSE("01\n", "01", "0\n");
  TEST_TRANSPOSE("11\n01\n", 0, "10\n11\n");
  TEST_TRANSPOSE("1110101\n"
                 "0011100\n"
                 "1011111\n",

                 "0010000",

                 "101\n100\n011\n111\n001\n101\n");

  string myBefore;
  string myRemove;
  string myAfter;
  for (size_t i = 0; i < 200; ++i) {
    myBefore += "0101";
    myRemove += "0011";
    myAfter += "0\n1\n";
  }
  TEST_TRANSPOSE(myBefore.c_str(), myRemove.c_str(), myAfter.c_str());
}
