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
#ifndef TEST_SORTER_GUARD
#define TEST_SORTER_GUARD

#include "TestVisitor.h"

class TestCase;

/** Sorts tests to avoid using the order imposed by the order of
 construction of global objects, since this is inconsistent across
 compilers and platforms.
*/
class TestSorter : public TestVisitor {
  virtual bool visitEnter(TestSuite& testSuite);
};

#endif
