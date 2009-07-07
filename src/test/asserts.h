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
#ifndef ASSERTS_GUARD
#define ASSERTS_GUARD

#include <stdexcept>
#include <sstream>

class AssertException : public logic_error {
 public:
  AssertException(const string& str);
  AssertException(const AssertException& e);
};

void assertSucceeded(bool printDot);

void assertFailed(const char* errorMsg,
				  const char* testName, const char* file, size_t line);

void assertTrue(bool value, const char* condition,
				const char* testName, const char* file, size_t line,
				bool printDot);
#define ASSERT_TRUE(VALUE) \
  assertTrue(VALUE, #VALUE, __nameOfTest, __FILE__, __LINE__, __printDots)

void assertFalse(bool value, const char* condition,
				 const char* testName, const char* file, size_t line,
				 bool printDot);
#define ASSERT_FALSE(VALUE) \
  assertFalse(VALUE, #VALUE, __nameOfTest, __FILE__, __LINE__, __printDots)

void assertEqualFailed(const char* a, const char* b,
					   const char* aString, const char* bString,
					   const char* testName, const char* file, size_t line);

template<class T1, class T2>
  void assertEqual(const T1& a, const T2& b,
				   const char* aString, const char* bString,
				   const char* testName, const char* file, size_t line,
				   bool printDot) {
  if (a == b) {
	assertSucceeded(printDot);
	return;
  }

  stringstream aValue;
  aValue << a;

  stringstream bValue;
  bValue << b;

  assertEqualFailed(aValue.str().c_str(), bValue.str().c_str(),
					aString, bString,
					testName, file, line);
}

#define ASSERT_EQ(A, B) \
  assertEqual(A, B, #A, #B, __nameOfTest, __FILE__, __LINE__, __printDots)

#endif
