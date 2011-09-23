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

#include "frobby.h"
#include "tests.h"
#include "IdealFactory.h"
#include "LibTest.h"
#include "BigIdeal.h"

#include <vector>

TEST_SUITE2(LibraryInterface, IrreducibleDecom)

TEST(IrreducibleDecom, TypicalEncoded) {
  Frobby::Ideal ideal = toLibIdeal(IdealFactory::xx_yy_xz_yz());
  LibIdealConsumer consumer(IdealFactory::ring_xyzt());

  bool returnValue =
    Frobby::irreducibleDecompositionAsMonomials(ideal, consumer);

  ASSERT_TRUE(returnValue);
  ASSERT_EQ(consumer.getIdeal(), IdealFactory::xy_xxyyz());
  ASSERT_TRUE(consumer.hasAnyOutput());
}

TEST(IrreducibleDecom, Typical) {
  Frobby::Ideal ideal = toLibIdeal(IdealFactory::xx_yy_xz_yz());
  LibIdealsConsumer consumer(IdealFactory::ring_xyzt());

  Frobby::irreducibleDecompositionAsIdeals(ideal, consumer);

  ASSERT_EQ(consumer.getIdeals(), IdealFactory::irrdecom_xx_yy_xz_yz());
}

TEST(IrreducibleDecom, ZeroIdealEncoded) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal frobbyInputIdeal(varCount);
    LibIdealConsumer consumer((VarNames(varCount)));

    bool returnValue =
      Frobby::irreducibleDecompositionAsMonomials(frobbyInputIdeal, consumer);

    ASSERT_FALSE(returnValue);
    ASSERT_FALSE(consumer.hasAnyOutput());
  }
}

TEST(IrreducibleDecom, ZeroIdeal) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal frobbyInputIdeal(varCount);
    LibIdealsConsumer consumer((VarNames(varCount)));

    Frobby::irreducibleDecompositionAsIdeals(frobbyInputIdeal, consumer);

    vector<BigIdeal> ideals;
    ideals.push_back(IdealFactory::zeroIdeal(varCount));
    ASSERT_EQ(consumer.getIdeals(), ideals);
  }
}

TEST(IrreducibleDecom, WholeRingEncoded) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal = toLibIdeal(IdealFactory::wholeRing(varCount));
    LibIdealConsumer consumer((VarNames(varCount)));

    bool returnValue =
      Frobby::irreducibleDecompositionAsMonomials(ideal, consumer);

    ASSERT_TRUE(returnValue);
    ASSERT_TRUE(consumer.hasAnyOutput());
    ASSERT_EQ(consumer.getIdeal(), IdealFactory::zeroIdeal(varCount));
  }
}

TEST(IrreducibleDecom, WholeRing) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal = toLibIdeal(IdealFactory::wholeRing(varCount));
    LibIdealsConsumer consumer((VarNames(varCount)));

    Frobby::irreducibleDecompositionAsIdeals(ideal, consumer);

    ASSERT_EQ(consumer.getIdeals(), vector<BigIdeal>());
  }
}
