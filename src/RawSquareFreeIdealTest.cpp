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

TEST(RawSquareFreeIdeal, NewRawSquareFreeIdealParse) {
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
