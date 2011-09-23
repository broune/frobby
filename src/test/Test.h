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
#ifndef TEST_GUARD
#define TEST_GUARD

#include <vector>
#include <string>

class TestVisitor;

/** This class represents a test or a suite of tests according to the
 Composite Pattern. The hierarchy of tests can be traversed according
 to the Visitor Pattern.
*/
class Test {
public:
  /** Construct a Test with the given name. */
  Test(const string& name);
  virtual ~Test();

  /** Makes the visitor visit this object as per the Visitor
      Pattern.
  */
  virtual bool accept(TestVisitor& visitor) = 0;

  /** Returns the name passed to the constructor. */
  const string& getName() const;

  /** Compares names lexicographically. */
  bool operator<(const Test& test) const;

 private:
  /** The name passed to the constructor. */
  string _name;
};

#endif
