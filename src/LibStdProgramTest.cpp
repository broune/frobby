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

#include "frobby.h"
#include "tests.h"
#include "BigIdeal.h"
#include "IdealFactory.h"
#include "LibTest.h"

TEST_SUITE2(LibraryInterface, StandardProgram)

TEST(StandardProgram, TypicalNonNegative) {
  vector<mpz_class> grading(4);
  grading[0] = 0;
  grading[1] = 100;
  grading[2] = 10000;
  grading[3] = "1000000000000000000000000000000000000";
  Frobby::Ideal ideal = toLibIdeal(IdealFactory::xx_yy_zz_t_xz_yz());
  LibIdealConsumer consumer(IdealFactory::ring_xyzt());

  bool returnValue =
    Frobby::solveStandardMonomialProgram
    (ideal, castLibArray(grading), consumer);

  ASSERT_TRUE(returnValue);
  ASSERT_EQ(consumer.getIdeal(), IdealFactory::z());
}

TEST(StandardProgram, TypicalNegative) {
  vector<mpz_class> grading(4);
  grading[0] = 0;
  grading[1] = 100;
  grading[2] = -10000;
  grading[3] = "1000000000000000000000000000000000000";
  Frobby::Ideal ideal = toLibIdeal(IdealFactory::xx_yy_zz_t_xz_yz());
  LibIdealConsumer consumer(IdealFactory::ring_xyzt());

  bool returnValue =
    Frobby::solveStandardMonomialProgram(ideal, castLibArray(grading), consumer);

  ASSERT_TRUE(returnValue);
  ASSERT_EQ(consumer.getIdeal(), IdealFactory::xy());
}

TEST(StandardProgram, ZeroIdeal) {
  vector<mpz_class> grading(3);
  grading[0] = 0;
  grading[1] = 100;
  grading[2] = -10000;

  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal(varCount);
    LibIdealConsumer consumer((VarNames(varCount)));

    bool returnValue =
      Frobby::solveStandardMonomialProgram
      (ideal, castLibArray(grading), consumer);

    if (varCount == 0) {
      ASSERT_TRUE(returnValue);
      ASSERT_EQ(consumer.getIdeal(), IdealFactory::wholeRing(varCount));
    } else {
      ASSERT_FALSE(returnValue);
      ASSERT_EQ(consumer.getIdeal(), IdealFactory::zeroIdeal(varCount));
    }
  }
}

TEST(StandardProgram, WholeRing) {
  vector<mpz_class> grading(3);
  grading[0] = 0;
  grading[1] = 100;
  grading[2] = -10000;

  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal = toLibIdeal(IdealFactory::wholeRing(varCount));
    LibIdealConsumer consumer((VarNames(varCount)));

    bool returnValue =
      Frobby::solveStandardMonomialProgram
      (ideal, castLibArray(grading), consumer);

    ASSERT_FALSE(returnValue);
    ASSERT_EQ(consumer.getIdeal(), IdealFactory::zeroIdeal(varCount));
  }
}
