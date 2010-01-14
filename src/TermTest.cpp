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
#include "Term.h"
#include "tests.h"

TEST_SUITE(Term)

TEST(Term, ParseStringConstructorNull) {
  ASSERT_EQ(Term(""), Term());
}

TEST(Term, ParseStringConstructor) {
  Term term(3);
  term[0] = 3;
  term[1] = 9;
  term[2] = 7;

  ASSERT_EQ(Term("3 9 7"), term);
}

TEST(Term, ShareNonZeroExponent) {
  ASSERT_TRUE(Term("0 1 2").sharesNonZeroExponent(Term("2 1 0")));
  ASSERT_FALSE(Term("0 1 2").sharesNonZeroExponent(Term("0 2 1")));
}
