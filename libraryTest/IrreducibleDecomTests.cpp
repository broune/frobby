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

#include "IrreducibleDecomTests.h"

#include "../src/frobby.h"
#include "MyAsserts.h"
#include "MyConsumers.h"
#include "MyIdealCreators.h"
#include "MyIdeal.h"

void testIrreducibleDecomIdeal() {
  {
	MyIdeal inputIdeal = make4_xx_yy_xz_yz();
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);
	
	MyIdealConsumer consumer;
	bool returnValue =
	  Frobby::irreducibleDecompositionAsMonomials(frobbyInputIdeal, consumer);

	assertTrue(returnValue,
			   "irrdecom as monomials of ideal returnValue");
	assertEqual(consumer.getIdeal(), make4_xy_xxyyz(),
				"irrdecom as monomials of ideal");
	assertTrue(consumer.hasAnyOutput(),
			   "irrdecom as monomials of ideal hasAnyOutput");
  }

  {
	MyIdeal inputIdeal = make4_xx_yy_xz_yz();
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyIdealsConsumer consumer;
	Frobby::irreducibleDecompositionAsIdeals(frobbyInputIdeal, consumer);

	assertEqual(consumer.getIdeals(), makeIrrdecom4_xx_yy_xz_yz(),
				"irrdecom as ideals of ideal");
  }
}

void testIrreducibleDecomZeroIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyIdealConsumer consumer;
	bool returnValue =
	  Frobby::irreducibleDecompositionAsMonomials(frobbyInputIdeal, consumer);

	assertTrue(!returnValue,
			   "irrdecom as monomials of decomZero returnValue");
	assertTrue(!consumer.hasAnyOutput(),
			   "irrdecom as monomials of decomZero hasAnyOutput");
  }

  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);

	MyIdealsConsumer consumer;
	Frobby::irreducibleDecompositionAsIdeals(frobbyInputIdeal, consumer);

	assertEqual(consumer.getIdeals(), makeIdeals(make_0()),
				"irrdecom as ideals of decomZero");
  }
}

void testIrreducibleDecomOneIdeal() {
  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyIdealConsumer consumer;
	bool returnValue =
	  Frobby::irreducibleDecompositionAsMonomials(frobbyInputIdeal, consumer);

	assertTrue(returnValue,
			   "irrdecom as monomials of one returnValue");
	assertTrue(consumer.hasAnyOutput(),
			   "irrdecom as monomials of one hasAnyOutput");
	assertEqual(consumer.getIdeal(), make_0(),
				"irrdecom as monomials of one");
  }

  for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

	MyIdealsConsumer consumer;
	Frobby::irreducibleDecompositionAsIdeals(frobbyInputIdeal, consumer);

	assertEqual(consumer.getIdeals(), MyIdeals(),
				"irrdecom as ideals of one");
  }
}

// Perform each of the tests above.
void testIrreducibleDecom() {
  testIrreducibleDecomIdeal();
  testIrreducibleDecomZeroIdeal();
  testIrreducibleDecomOneIdeal();
}
