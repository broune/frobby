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

// This file shows how to compute Alexander Duals using Frobby. This
// is done by way of a number of tests of the computations performed.

// Do a test where the point used for the Alexander dual is given
// explicitly.
void testAlexanderDualExplicitPoint() {
  MyIdeal inputIdeal = make4_xx_yy_xz_yz();
  Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

  MyIdealConsumer consumer;
  mpz_t pointLcm[4];
  mpz_init_set_si(pointLcm[0], 2);
  mpz_init_set_si(pointLcm[1], 2);
  mpz_init_set_si(pointLcm[2], 3);
  mpz_init_set_si(pointLcm[3], 100);

  Frobby::alexanderDual(frobbyInputIdeal, pointLcm, consumer);

  mpz_clear(pointLcm[0]);
  mpz_clear(pointLcm[1]);
  mpz_clear(pointLcm[2]);
  mpz_clear(pointLcm[3]);

  assertEqual(consumer.getIdeal(), make4_xyzzz_xxyy(),
			  "alexdual explicit of ideal");
}

// Do a test where the point used for the Alexander dual is not given
// explicitly.
void testAlexanderDualImplicitPoint() {
  MyIdeal inputIdeal = make4_xx_yy_xz_yz();
  Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

  MyIdealConsumer consumer;
  Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);

  assertEqual(consumer.getIdeal(), make4_xyz_xxyy(),
			  "alexdual implicit of ideal");
}

// Do a test of the edge case of computing the Alexander dual of the
// zero ideal in polynomial rings with 0, 1, 2 and 3 variables.
void testAlexanderDualZeroIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyIdealConsumer consumer;
	Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);

	assertEqual(consumer.getIdeal(), make_0(), "alexdual of zero");
  }
}

// Do a test of the edge case of copmuting the Alexander dual of the
// ideal generated by 1 in polynomial rings with 0, 1, 2 and 3
// variables.
void testAlexanderDualOneIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyIdealConsumer consumer;
	Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);

	assertEqual(consumer.getIdeal(), make_1(varCount), "alexdual of one");
  }
}

// Perform each of the tests above.
void testAlexanderDual() {
  testAlexanderDualExplicitPoint();
  testAlexanderDualImplicitPoint();
  testAlexanderDualZeroIdeal();
  testAlexanderDualOneIdeal();
}
