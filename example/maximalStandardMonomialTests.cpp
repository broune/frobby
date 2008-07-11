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

#include "maximalStandardMonomialTests.h"

#include "../src/frobby.h"
#include "MyAsserts.h"
#include "MyConsumers.h"
#include "MyIdealCreators.h"
#include "MyIdeal.h"

void testMsmIdeal() {
  MyIdeal inputIdeal = make4_xx_yy_zz_t_xz_yz();
  Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

  MyIdealConsumer consumer;
  Frobby::maximalStandardMonomials(frobbyInputIdeal, consumer);

  assertEqual(consumer.getIdeal(), make4_xy_z(), "msm of ideal");
}

void testMsmZeroIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyIdealConsumer consumer;
	Frobby::maximalStandardMonomials(frobbyInputIdeal, consumer);

	if (varCount == 0)
	  assertEqual(consumer.getIdeal(), make_1(0), "msm of zero (zero)");
	else
	  assertEqual(consumer.getIdeal(), make_0(), "msm of zero (more)");
  }
}

void testMsmOneIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyIdealConsumer consumer;
	Frobby::maximalStandardMonomials(frobbyInputIdeal, consumer);

	assertEqual(consumer.getIdeal(), make_0(), "msm of one");
  }
}

void testMsm() {
  testMsmIdeal();
  testMsmZeroIdeal();
  testMsmOneIdeal();
}
