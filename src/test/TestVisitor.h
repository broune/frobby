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
#ifndef TEST_VISITOR_GUARD
#define TEST_VISITOR_GUARD

class TestSuite;
class TestCase;

/** This class is a visitor for classes derived from Test according to
 the Visitor Pattern.
*/
class TestVisitor {
public:
  virtual ~TestVisitor();

  /** Returns true if the testSuite should be iterated through. Is
   called before testSuite is processed in any other way, and if
   return value is false, the testSuite will not be iterated through,
   according to the default implemenation of visit(TestCase&). The
   default implementation always returns true.
  */
  virtual bool visitEnter(TestSuite& testSuite);

  /** Visits a testCase and returns true if visiting of any other
   objects should continue after the testSuite has been visited. The
   visiting process will stop if return value is false, according to
   the default implementation of visit(TestSuite&). The default
   implementation always returns true.
  */
  virtual bool visit(TestCase& testCase);

  /** The default implementation calls visitEnter and visits the added
   objects in forward order if the return value is true. If no visit
   returns false, then calls visitLeave and returns its return
   value. Returns false and visits no more objects and does not call
   visitLeave if return value of any visit is false. The return value
   of visitEnter only governs whether to iterate through the added
   tests, it does not prevent calling visitLeave and it does not
   directly influence the return value.

   Usually it will be sufficient to overload visitEnter and
   visitLeave, as opposed to overloading this method.
  */
  virtual bool visit(TestSuite& testSuite);

  /** Returns true if visiting of any other objects should continue
   after the testSuite has been visited. Is called as last step of
   visiting testSuite, and the visiting process will stop if return
   value is false, according to the default implementation of
   visit(TestSuite&). The default implementation always returns true.
  */
  virtual bool visitLeave(TestSuite&);
};

#endif
