/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

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
#include "TermTranslator.h"
#include "tests.h"

TEST_SUITE(TermTranslator)

TEST(TermTranslator, IdentityConstructor) {
  TermTranslator translator(3, 10);
  ASSERT_EQ(translator.toString(),
			"TermTranslator(\n"
			" var 1: 0 1 2 3 4 5 6 7 8 9 0\n"
			" var 2: 0 1 2 3 4 5 6 7 8 9 0\n"
			" var 3: 0 1 2 3 4 5 6 7 8 9 0\n"
			")\n");
}
