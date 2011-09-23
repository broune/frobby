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

TEST_SUITE2(LibraryInterface, MaxStd)

TEST(MaxStd, Typical) {
  Frobby::Ideal ideal = toLibIdeal(IdealFactory::xx_yy_zz_t_xz_yz());
  LibIdealConsumer consumer(IdealFactory::ring_xyzt());

  Frobby::maximalStandardMonomials(ideal, consumer);

  ASSERT_EQ(consumer.getIdeal(), IdealFactory::xy_z());
}

TEST(MaxStd, ZeroIdeal) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal(varCount);
    LibIdealConsumer consumer((VarNames(varCount)));

    Frobby::maximalStandardMonomials(ideal, consumer);

    if (varCount == 0) {
      ASSERT_EQ(consumer.getIdeal(), IdealFactory::wholeRing(varCount));
    } else {
      ASSERT_EQ(consumer.getIdeal(), IdealFactory::zeroIdeal(varCount));
    }
  }
}

TEST(MaxStd, WholeRing) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal = toLibIdeal(IdealFactory::wholeRing(varCount));
    LibIdealConsumer consumer((VarNames(varCount)));

    Frobby::maximalStandardMonomials(ideal, consumer);

    ASSERT_EQ(consumer.getIdeal(), IdealFactory::zeroIdeal(varCount));
  }
}
