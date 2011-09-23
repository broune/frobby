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
#include "TestSuite.h"

#include "TestVisitor.h"
#include <algorithm>

TestSuite::TestSuite(const string& name):
  Test(name) {
}

void TestSuite::add(Test* test) {
  ASSERT(test != 0);
  _tests.push_back(test);
}

namespace {
  /** Follows pointers before comparing values. Helper function for
      TestSuite::sortTests.
   */
  bool comparePointedToValue(const Test* a, const Test* b) {
    ASSERT(a != 0);
    ASSERT(b != 0);
    return *a < *b;
  }
}

void TestSuite::sortTests() {
  std::sort(begin(), end(), comparePointedToValue);
}

TestSuite::TestIterator TestSuite::begin() {
  return _tests.begin();
}

TestSuite::TestIterator TestSuite::end() {
  return _tests.end();
}

bool TestSuite::accept(TestVisitor& visitor) {
  return visitor.visit(*this);
}
