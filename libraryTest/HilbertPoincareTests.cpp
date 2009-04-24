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

#include "HilbertPoincareTests.h"

#include "../src/frobby.h"
#include "MyAsserts.h"
#include "MyIdealCreators.h"
#include "MyIdeal.h"
#include "MyConsumers.h"
#include "MyPolynomial.h"
#include "MyPolynomialCreators.h"

void testHilbertPoincareIdeal() {
  MyIdeal inputIdeal = make4_xx_yy_xz_yz();

  {
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyPolynomialConsumer consumer;
	Frobby::multigradedHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makeHilbert4_xx_yy_xz_yz(),
				"multigraded hilbert ideal");
  }

  {
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyPolynomialConsumer consumer;
	Frobby::univariateHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makePoly1_1_m4to2_3to3_1to4_m1to5(),
				"univariate hilbert ideal");
  }
}

void testHilbertPoincareZeroIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyPolynomialConsumer consumer;
	Frobby::multigradedHilbertPoincareSeries(frobbyInputIdeal, consumer);
	
	assertEqual(consumer.getPolynomial(), makePoly_1(varCount),
				"multigraded hilbert zero");
  }

  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyPolynomialConsumer consumer;
	Frobby::univariateHilbertPoincareSeries(frobbyInputIdeal, consumer);
	
	assertEqual(consumer.getPolynomial(), makePoly_1(1),
				"univariate hilbert zero");
  }
}

void testHilbertPoincareOneIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyPolynomialConsumer consumer;
	Frobby::multigradedHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makePoly_0(),
				"multigraded hilbert one");
  }

  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyPolynomialConsumer consumer;
	Frobby::univariateHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makePoly_0(),
				"univariate hilbert one");
  }
}

// Perform each of the tests above.
void testHilbertPoincareSeries() {
  testHilbertPoincareIdeal();
  testHilbertPoincareOneIdeal();
  testHilbertPoincareZeroIdeal();
}
