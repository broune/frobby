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
#include "all.h"

#include "TestRunner.h"
#include "asserts.h"

TEST(root, qbob) {
  cout << "inside qbob" << endl;
}

TEST(bob1, bab1) {
  cout << "inside bob11" << endl;
}
TEST(bob1, bab2) {
  cout << "inside bob12" << endl;
}
TEST(bob2, bab1) {
  cout << "inside bob21" << endl;
  ASSERT_EQ(3,4);
}

TEST(bob2, bab2) {
  cout << "inside bob22" << endl;
}

TEST_SUITE2(bob2,bob1)
TEST_SUITE(bob2)

int main() {
  try {
  TestRunner runner;
  GET_TEST_SUITE(root).accept(runner);
  } catch (std::exception& e) {
    cout << "caught exception!" << endl;
    cout << e.what() << endl;
  }

  return 0;
}
