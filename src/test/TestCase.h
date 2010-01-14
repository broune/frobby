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
#ifndef TEST_CASE_GUARD
#define TEST_CASE_GUARD

#include "Test.h"

class TestVisitor;

/** Represents a test case, which is usually created through a macro
 that defines a subclass. This class is a leaf of the Composite
 Pattern, and can be visited by a TestVisitor according to the Visitor
 Pattern.
*/
class TestCase : public Test {
 public:
  /** Construct a test case with the given name. */
  TestCase(const string& name);

  virtual bool accept(TestVisitor& visitor);

  /** Run the test and record the name of the test as
   __nameOfTest. This can be a more detailed name, and it can be
   derived from the name that is returned by getName(), or it can be
   unrelated. This provides needed flexibility. printDots indicates
   whether to print dots when individual assertions succeed.
  */
  virtual void run(const char* nameOfTest, bool printDots) = 0;
};

#endif
