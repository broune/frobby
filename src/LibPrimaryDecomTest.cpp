/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
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

#include "frobby.h"
#include "tests.h"
#include "IdealFactory.h"
#include "LibTest.h"
#include "BigIdeal.h"

#include <vector>

TEST_SUITE2(LibraryInterface, PrimaryDecom)

TEST(PrimaryDecom, Typical) {
  Frobby::Ideal ideal = toLibIdeal(IdealFactory::xx_yy_xz_yz());
  LibIdealsConsumer consumer(IdealFactory::ring_xyzt());

  Frobby::primaryDecomposition(ideal, consumer);

  ASSERT_EQ(consumer.getIdeals(), IdealFactory::irrdecom_xx_yy_xz_yz());
}

TEST(PrimaryDecom, ZeroIdeal) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal frobbyInputIdeal(varCount);
    LibIdealsConsumer consumer((VarNames(varCount)));

    Frobby::primaryDecomposition(frobbyInputIdeal, consumer);

    vector<BigIdeal> ideals;
    ideals.push_back(IdealFactory::zeroIdeal(varCount));
    ASSERT_EQ(consumer.getIdeals(), ideals);
  }
}

TEST(PrimaryDecom, WholeRing) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal = toLibIdeal(IdealFactory::wholeRing(varCount));
    LibIdealsConsumer consumer((VarNames(varCount)));

    Frobby::primaryDecomposition(ideal, consumer);

    ASSERT_EQ(consumer.getIdeals(), vector<BigIdeal>());
  }
}
