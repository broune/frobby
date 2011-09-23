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
#include "TermGrader.h"
#include "tests.h"

#include "TermTranslator.h"
#include "Term.h"

TEST_SUITE(TermGrader)

TEST(TermGrader, getUpperBound) {
    vector<mpz_class> v(3);
    v[0] = -10;
    v[1] = 0;
    v[2] = 10;
    TermGrader grader(v, TermTranslator(3, 9));

    // Simple cases.
    ASSERT_EQ(grader.getUpperBound(Term("0 0 0"), Term("0 0 0")),   0);
    ASSERT_EQ(grader.getUpperBound(Term("1 0 0"), Term("2 0 0")), -10);
    ASSERT_EQ(grader.getUpperBound(Term("0 1 0"), Term("0 2 0")),   0);
    ASSERT_EQ(grader.getUpperBound(Term("0 0 1"), Term("0 0 2")),  20);
    ASSERT_EQ(grader.getUpperBound(Term("1 1 1"), Term("2 2 2")),  10);

    // Test handling of highest value mapping to zero.
    ASSERT_EQ(grader.getUpperBound(Term("9 9 9"), Term("9 9 9")),  0);
    ASSERT_EQ(grader.getUpperBound(Term("1 0 0"), Term("9 0 0")),  0);
    ASSERT_EQ(grader.getUpperBound(Term("0 0 0"), Term("0 9 0")),  0);
    ASSERT_EQ(grader.getUpperBound(Term("0 0 0"), Term("0 0 9")), 80);
    ASSERT_EQ(grader.getUpperBound(Term("1 1 1"), Term("9 9 9")), 80);
}

#define MIN_INDEX_TEST(from, to, maxDegree, strict, expectFind, expectedIndex) \
  { \
    Exponent foundIndex = 0; \
    bool returnValue = grader.getMinIndexLessThan \
      (0, from, to, foundIndex, maxDegree - strict); \
    if (expectFind) { \
      ASSERT_TRUE(returnValue); \
      ASSERT_EQ(foundIndex, (Exponent)expectedIndex);   \
    } else { \
      ASSERT_FALSE(returnValue); \
    } \
  }

TEST(TermGrader, getMinIndexLessThanNegative) {
  vector<mpz_class> v;
  v.push_back(-2);
  TermGrader grader(v, TermTranslator(1, 10));

  // General case of represented degree
  MIN_INDEX_TEST(0, 10, -12, true, true, 7); // -14 < -12
  MIN_INDEX_TEST(0, 10, -12, false, true, 6); // -12 <= -12

  // General case not of represented degree
  MIN_INDEX_TEST(0, 10, -5, true, true, 3); // -6 < -5
  MIN_INDEX_TEST(0, 10, -5, false, true, 3); // -6 <= -5

  // The zero at 0
  MIN_INDEX_TEST(0, 10, 0, true, true, 1); // -2 < 0
  MIN_INDEX_TEST(0, 10, 0, false, true, 0); // 0 <= 0

  // The zero at 10
  MIN_INDEX_TEST(10, 10, 1, true, true, 10); // 0 < 1
  MIN_INDEX_TEST(10, 10, 0, false, true, 10); // 0 <= 0

  // Left of interval
  MIN_INDEX_TEST(4, 10, -6, true, true, 4);
  MIN_INDEX_TEST(4, 10, -5, false, true, 4);

  // Right of interval
  MIN_INDEX_TEST(0, 3, -6, true, false, 0); // no [0,-6] < -6
  MIN_INDEX_TEST(0, 3, -7, false, false, 0); // no [0,-6] <= -7

  // Empty interval
  MIN_INDEX_TEST(10, 0, -5, true, false, 0);
  MIN_INDEX_TEST(10, 0, -5, false, false, 0);
}

TEST(TermGrader, getMinIndexLessThanPositive) {
  vector<mpz_class> v;
  v.push_back(2);
  TermGrader grader(v, TermTranslator(1, 10));

  // General case
  MIN_INDEX_TEST(0, 10, 12, true, true, 0); // 0 < 12
  MIN_INDEX_TEST(0, 10, 12, false, true, 0); // 0 <= 12

  // The zero at 0
  MIN_INDEX_TEST(0, 10, 0, true, false, 0); // 0 not < [0,18]
  MIN_INDEX_TEST(0, 10, 0, false, true, 0); // 0 <= 0

  // The zero at 10
  MIN_INDEX_TEST(10, 10, 0, true, false, 0); // 0 not < 0
  MIN_INDEX_TEST(10, 10, 0, false, true, 10); // 0 <= 0

  // Search at -1.
  MIN_INDEX_TEST(0, 10, -1, true, false, 0);
  MIN_INDEX_TEST(0, 10, -1, false, false, 0);

  // Empty interval
  MIN_INDEX_TEST(10, 0, -5, true, false, 0);
  MIN_INDEX_TEST(10, 0, -5, false, false, 0);
}

TEST(TermGrader, getMinIndexLessThanZero) {
  vector<mpz_class> v;
  v.push_back(0);
  TermGrader grader(v, TermTranslator(1, 10));

  // Search above 0.
  MIN_INDEX_TEST(5, 10, 1, true, true, 5);
  MIN_INDEX_TEST(5, 10, 1, false, true, 5);

  // Search at 0.
  MIN_INDEX_TEST(5, 10, 0, true, false, 0);
  MIN_INDEX_TEST(5, 10, 0, false, true, 5);

  // Search at -1.
  MIN_INDEX_TEST(0, 10, -1, true, false, 0);
  MIN_INDEX_TEST(0, 10, -1, false, false, 0);

  // Empty interval
  MIN_INDEX_TEST(10, 0, -5, true, false, 0);
  MIN_INDEX_TEST(10, 0, -5, false, false, 0);
}



#define MAX_INDEX_TEST(from, to, maxDegree, strict, expectFind, expectedIndex) \
  { \
    Exponent foundIndex = 0; \
    bool returnValue = grader.getMaxIndexLessThan \
      (0, from, to, foundIndex, maxDegree - strict); \
    if (expectFind) { \
      ASSERT_TRUE(returnValue); \
      ASSERT_EQ(foundIndex, (Exponent)expectedIndex);   \
    } else { \
      ASSERT_FALSE(returnValue); \
    } \
  }

TEST(TermGrader, getMaxIndexLessThanNegative) {
  vector<mpz_class> v;
  v.push_back(-2);
  TermGrader grader(v, TermTranslator(1, 10));

  // General case
  MAX_INDEX_TEST(0, 10, -12, true, true, 9); // -18 < -12
  MAX_INDEX_TEST(0, 10, -12, false, true, 9); // -18 <= 12

  // The zero at 10
  MAX_INDEX_TEST(10, 10, 0, true, false, 0); // 0 not < 0
  MAX_INDEX_TEST(0, 10, 0, false, true, 10); // 0 <= 0

  // The zero at 0
  MAX_INDEX_TEST(0, 0, 0, true, false, 0); // 0 not < 0
  MAX_INDEX_TEST(0, 0, 0, false, true, 0); // 0 <= 0
}

TEST(TermGrader, getMaxIndexLessThanPositive) {
  vector<mpz_class> v;
  v.push_back(2);
  TermGrader grader(v, TermTranslator(1, 10));

  // General case of represented degree
  MAX_INDEX_TEST(0, 9, 12, true, true, 5); // 10 < 12
  MAX_INDEX_TEST(0, 9, 12, false, true, 6); // 12 <= 12

  // General case not of represented degree
  MAX_INDEX_TEST(0, 9, 5, true, true, 2); // 4 < 5
  MAX_INDEX_TEST(0, 9, 5, false, true, 2); // 4 <= 5

  // The zero at 0
  MAX_INDEX_TEST(0, 9, 1, true, true, 0); // 0 < 1
  MAX_INDEX_TEST(0, 9, 0, false, true, 0); // 0 <= 0

  // The zero at 10
  MAX_INDEX_TEST(0, 10, 1, true, true, 10); // 0 < 1
  MAX_INDEX_TEST(0, 10, 0, false, true, 10); // 0 <= 0

  // Left of interval
  MAX_INDEX_TEST(4, 9, 7, true, false, 0); // no [8, 18] < 7
  MAX_INDEX_TEST(4, 9, 7, false, false, 0); // no [8, 18] <= 7

  // Right of interval
  MAX_INDEX_TEST(1, 5, 12, true, true, 5); // 10 < 12
  MAX_INDEX_TEST(1, 5, 12, false, true, 5); // 10 <= 12

  // Search at -1.
  MAX_INDEX_TEST(0, 10, -1, true, false, 0); // no [0,18] < -1
  MAX_INDEX_TEST(0, 10, -1, false, false, 0); // no [0,18] <= -1
}

TEST(TermGrader, getMaxIndexLessThanZero) {
  vector<mpz_class> v;
  v.push_back(0);
  TermGrader grader(v, TermTranslator(1, 10));

  // Search above 0.
  MAX_INDEX_TEST(5, 10, 1, true, true, 10);
  MAX_INDEX_TEST(5, 10, 1, false, true, 10);

  // Search at 0.
  MAX_INDEX_TEST(5, 10, 0, true, false, 0);
  MAX_INDEX_TEST(5, 10, 0, false, true, 10);

  // Search at -1.
  MAX_INDEX_TEST(0, 10, -1, true, false, 0);
  MAX_INDEX_TEST(0, 10, -1, false, false, 0);
}
