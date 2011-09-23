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
#include "Ideal.h"
#include "tests.h"

#include "Term.h"

TEST_SUITE(Ideal)

TEST(Ideal, IsWeaklyGeneric1) {
  Ideal id(4);
  id.insert(Term("0 2 1 1"));
  id.insert(Term("0 1 2 2"));
  id.insert(Term("0 3 2 0"));

  // The first generator strictly divides the lcm of the other two.
  ASSERT_TRUE(id.isWeaklyGeneric());
}

TEST(Ideal, IsWeaklyGeneric2) {
  Ideal id(4);
  id.insert(Term("0 2 1 1"));
  id.insert(Term("0 1 2 2"));
  id.insert(Term("0 2 2 0"));

  ASSERT_FALSE(id.isWeaklyGeneric());
}
