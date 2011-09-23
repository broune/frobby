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
#include "BigPolynomial.h"
#include "IdealFactory.h"
#include "PolynomialFactory.h"
#include "LibTest.h"

TEST_SUITE2(LibraryInterface, HilbertPoincare)

TEST(HilbertPoincare, TypicalMultigraded) {
  Frobby::Ideal ideal = toLibIdeal(IdealFactory::xx_yy_xz_yz());
  LibPolynomialConsumer consumer(IdealFactory::ring_xyzt());

  Frobby::multigradedHilbertPoincareSeries(ideal, consumer);

  ASSERT_EQ(consumer.getPolynomial(), PolynomialFactory::hilbert_xx_yy_xz_yz());
}

TEST(HilbertPoincare, TypicalUnivariate) {
  Frobby::Ideal ideal = toLibIdeal(IdealFactory::xx_yy_xz_yz());
  BigPolynomial output = PolynomialFactory::one_minus4tt_3ttt_tttt_minusttttt();
  LibPolynomialConsumer consumer(output.getNames());

  Frobby::univariateHilbertPoincareSeries(ideal, consumer);

  ASSERT_EQ(consumer.getPolynomial(), output);
}

TEST(HilbertPoincare, ZeroIdealMultigraded) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal(varCount);
    LibPolynomialConsumer consumer((VarNames(varCount)));

    Frobby::multigradedHilbertPoincareSeries(ideal, consumer);

    ASSERT_EQ(consumer.getPolynomial(), PolynomialFactory::one(varCount));
  }
}

TEST(HilbertPoincare, ZeroIdealUnivariate) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal(varCount);
    LibPolynomialConsumer consumer((VarNames(1)));

    Frobby::univariateHilbertPoincareSeries(ideal, consumer);

    ASSERT_EQ(consumer.getPolynomial(), PolynomialFactory::one(1));
  }
}

TEST(HilbertPoincare, WholeRingMultigraded) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal = toLibIdeal(IdealFactory::wholeRing(varCount));
    LibPolynomialConsumer consumer((VarNames(varCount)));

    Frobby::multigradedHilbertPoincareSeries(ideal, consumer);

    ASSERT_EQ(consumer.getPolynomial(), PolynomialFactory::zero(varCount));
  }
}

TEST(HilbertPoincare, WholeRingUnivariate) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
    Frobby::Ideal ideal = toLibIdeal(IdealFactory::wholeRing(varCount));
    LibPolynomialConsumer consumer((VarNames(1)));

    Frobby::univariateHilbertPoincareSeries(ideal, consumer);

    ASSERT_EQ(consumer.getPolynomial(), PolynomialFactory::zero(1));
  }
}
