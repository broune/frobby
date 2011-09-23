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
#include "BigIdeal.h"
#include "IdealFactory.h"
#include "LibTest.h"

TEST_SUITE2(LibraryInterface, DimensionAndCodimension)

namespace {
  /** Helper function for tests of Frobby::dimension. */
  mpz_class dimension(const BigIdeal& ideal) {
    mpz_class dim;
    Frobby::dimension(toLibIdeal(ideal), dim.get_mpz_t());
    return dim;
  }

  /** Helper function for tests of Frobby::codimension. */
  mpz_class codimension(const BigIdeal& ideal) {
    mpz_class codim;
    Frobby::codimension(toLibIdeal(ideal), codim.get_mpz_t());
    return codim;
  }
}

TEST(DimensionAndCodimension, WholeRing) {
  ASSERT_EQ(dimension(IdealFactory::wholeRing(0)), -1);
  ASSERT_EQ(codimension(IdealFactory::wholeRing(0)), 1);

  ASSERT_EQ(dimension(IdealFactory::wholeRing(5)), -1);
  ASSERT_EQ(codimension(IdealFactory::wholeRing(5)), 6);
}

TEST(DimensionAndCodimension, ZeroIdeal) {
  ASSERT_EQ(dimension(IdealFactory::zeroIdeal(0)), 0);
  ASSERT_EQ(codimension(IdealFactory::zeroIdeal(0)), 0);

  ASSERT_EQ(dimension(IdealFactory::zeroIdeal(5)), 5);
  ASSERT_EQ(codimension(IdealFactory::zeroIdeal(5)), 0);
}

TEST(DimensionAndCodimension, Typical) {
  ASSERT_EQ(dimension(IdealFactory::xy_xz()), 3);
  ASSERT_EQ(codimension(IdealFactory::xy_xz()), 1);

  ASSERT_EQ(dimension(IdealFactory::xx_yy_xz_yz()), 2);
  ASSERT_EQ(codimension(IdealFactory::xx_yy_xz_yz()), 2);
}
