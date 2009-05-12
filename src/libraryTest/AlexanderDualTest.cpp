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

#include "../frobby.h"
#include "MyAsserts.h"
#include "MyIdealCreators.h"
#include "MyIdeal.h"
#include "MyConsumers.h"

#include "../test/all.h"
#include "../BigIdeal.h"

TEST_SUITE(LibraryInterface)
TEST_SUITE2(LibraryInterface, AlexanderDual)

// The point used for the Alexander dual is implicit.
TEST(AlexanderDual, ImplicitPoint) {
  BigIdeal ideal = makeb_xx_yy_xz_yz();
  Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(ideal);

  MyIdealConsumer2 consumer(makeRing_xyzt());
  Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);

  ASSERT_EQ(consumer.getIdeal(), makeb_xyz_xxyy());
}

// The point used for the Alexander dual is explicit.
TEST(AlexanderDual, ExplicitPoint) {
  Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(makeb_xx_yy_xz_yz());

  MyIdealConsumer2 consumer(makeRing_xyzt());
  vector<mpz_class> pointLcm(4);
  pointLcm[0] = 2;
  pointLcm[1] = 2;
  pointLcm[2] = 3;
  pointLcm[3] = 100;

  // The following cast depends on knowing that mpz_class just
  // consists of a single mpz_t, so assert that that is true.
  ASSERT(sizeof(mpz_class) == sizeof(mpz_t));
  mpz_t* rawLcmPointer = reinterpret_cast<mpz_t*>(&(pointLcm[0]));

  Frobby::alexanderDual(frobbyInputIdeal, rawLcmPointer, consumer);

  ASSERT_EQ(consumer.getIdeal(), makeb_xyzzz_xxyy());
}

// The edge case of the Alexander dual of the zero ideal.
TEST(AlexanderDual, ZeroIdeal) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyIdealConsumer2 consumer((VarNames(varCount)));
	Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);

	ASSERT_EQ(consumer.getIdeal(), makeb_1(varCount));
  }
}

// The edge case of the Alexander dual of the whole ring.
TEST(AlexanderDual, WholeRing) {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	BigIdeal ideal = makeb_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(ideal);

	MyIdealConsumer2 consumer(makeRing_xyzt());
	Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);

	ASSERT_EQ(consumer.getIdeal(), makeb_0(varCount));
  }  
}
