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
#ifndef TEST_SUITE_GUARD
#define TEST_SUITE_GUARD

#include "Test.h"
class TestVisitor;

/** Represents a collection of tests, be they individual tests or yet
 other collections of tests. This class is a composite as per the
 Composite pattern, and can be visited by a TestVisitor according to
 the Visitor Pattern.
*/
class TestSuite : public Test {
 public:
  /** Constructs a test suite with the given name. */
  TestSuite(const string& name);

  /** Adds a test to the test suite. This can be another TestSuite or
   an individual TestCase.
  */
  void add(Test* test);

  /** Sorts the added tests in some canonical order. */
  void sortTests();

  /** An iterator for the added tests*/
  typedef vector<Test*>::iterator TestIterator;

  /** Returns an iterator to the beginning of the range of added
   tests.
  */
  TestIterator begin();

  /** Returns an iterator just past the end of the range of added
   tests.
  */
  TestIterator end();

  virtual bool accept(TestVisitor& visitor);

 private:
  /** The collection of tests of added tests. */
  vector<Test*> _tests;
};

#endif
