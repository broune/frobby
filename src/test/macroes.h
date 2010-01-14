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
#ifndef MACROES_GUARD
#define MACROES_GUARD

#define GET_TEST_SUITE(SUITE) \
  getTestSuite_##SUITE##_()

class TestSuite;
TestSuite& GET_TEST_SUITE(root);

#define TEST_SUITE(SUITE) TEST_SUITE2(root, SUITE)

#define TEST_SUITE2(PARENT, SUITE) \
  TestSuite& GET_TEST_SUITE(PARENT); \
  TestSuite& GET_TEST_SUITE(SUITE) { \
    static TestSuite suite(#SUITE); \
    return suite; \
  } \
  class _Helper_Suite_##SUITE { \
   public: \
    _Helper_Suite_##SUITE() { \
      GET_TEST_SUITE(PARENT).add(&GET_TEST_SUITE(SUITE));   \
    } \
  } _Helper_Suite_##SUITE;

#define TEST(SUITE, TEST_NAME) \
  TestSuite& GET_TEST_SUITE(SUITE); \
  class _testCase_##SUITE##_##TEST_NAME : public TestCase { \
   public: \
    void run(const char* __nameOfTest, bool _printDots); \
    _testCase_##SUITE##_##TEST_NAME(): \
      TestCase(#TEST_NAME) {} \
  }; \
  class _Helper_testCase_##SUITE##_##TEST_NAME { \
   public: \
    _Helper_testCase_##SUITE##_##TEST_NAME() { \
      static _testCase_##SUITE##_##TEST_NAME test; \
      GET_TEST_SUITE(SUITE).add(&test); \
    } \
  } static _helper_testCase_##SUITE##_##TEST_NAME; \
  void _testCase_##SUITE##_##TEST_NAME::run \
    (const char* __nameOfTest, bool __printDots)

#define FRIEND_TEST(SUITE, TEST_NAME) \
  friend class _testCase_##SUITE##_##TEST_NAME

#endif
