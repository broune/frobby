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

// This file shows how to compute multigraded and univariate
// Hilbert-Poincare series using Frobby. This is done by way of a
// number of tests of the computations performed.

void testHilbertPoincareIdeal() {
  size_t varCount = 4;

  MyIdeal inputIdeal = make4_xx_yy_xz_yz();

  {
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyTermConsumer consumer(varCount);
	Frobby::multigradedHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makeHilbert4_xx_yy_xz_yz());
  }

  {
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyTermConsumer consumer(1);
	Frobby::univariateHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makePoly1_1_m4to2_3to3_1to4_m1to5());
  }
}

void testHilbertPoincareZeroIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyTermConsumer consumer(varCount);
	Frobby::multigradedHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makePoly_1(varCount));
  }

  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyTermConsumer consumer(1);
	Frobby::univariateHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makePoly_1(1));
  }
}

void testHilbertPoincareOneIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyTermConsumer consumer(varCount);
	Frobby::multigradedHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makePoly_0());
  }

  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyTermConsumer consumer(1);
	Frobby::univariateHilbertPoincareSeries(frobbyInputIdeal, consumer);

	assertEqual(consumer.getPolynomial(), makePoly_0());
  }
}

// Perform each of the tests above.
void testHilbertPoincareSeries() {
  testHilbertPoincareIdeal();
  testHilbertPoincareOneIdeal();
  testHilbertPoincareZeroIdeal();
}
