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
#include "TestVisitor.h"

#include "TestSuite.h"

TestVisitor::~TestVisitor() {
}

bool TestVisitor::visitEnter(TestSuite&) {
  return true;
}

bool TestVisitor::visit(TestCase& testCase) {
  return true;
}

bool TestVisitor::visit(TestSuite& testSuite) {
  if (visitEnter(testSuite)) {
    TestSuite::TestIterator end = testSuite.end();
    for (TestSuite::TestIterator it = testSuite.begin(); it != end; ++it)
      if (!(*it)->accept(*this))
        return false;
  }

  return visitLeave(testSuite);
}

bool TestVisitor::visitLeave(TestSuite&) {
  return true;
}
