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
#include "FrobbyStringStream.h"

#include <algorithm>
#include <exception>

// This is a replacement for stringstream, which may seem weird since
// stringstream should work perfectly fine for any purpose where
// FrobbyStringStream could be used. Unfortunately this is not the
// case.
//
// The sad truth is that stringstream is unusable due to its behavior
// when running out of memory - it just stops what it is doing and
// then returns without propagating the bad_alloc exception. Setting
// exception flags with the exception() method does not change this
// behavior. This is for GCC v. 3.4.4 and on GCC 4.1.2 I have tested
// that at least without setting exception() it fails silently, though
// not whether that can be changed by using exception().
//
// The advantage of FrobbyStringStream is that it does not try to be
// clever in any way, and so it avoids these issues.

FrobbyStringStream& FrobbyStringStream::operator<<(char character) {
  _str += character;
  return *this;
}

FrobbyStringStream& FrobbyStringStream::operator<<(unsigned long integer) {
  appendIntegerToString(_str, integer);
  return *this;
}

FrobbyStringStream& FrobbyStringStream::operator<<(unsigned int integer) {
  appendIntegerToString(_str, integer);
  return *this;
}

FrobbyStringStream& FrobbyStringStream::operator<<(const mpz_class& integer) {
  appendIntegerToString(_str, integer);
  return *this;
}

FrobbyStringStream& FrobbyStringStream::operator<<(const string& text) {
  _str += text;
  return *this;
}

FrobbyStringStream& FrobbyStringStream::operator<<(const char* text) {
  _str += text;
  return *this;
}

string& FrobbyStringStream::str() {
  return _str;
}

const string& FrobbyStringStream::str() const {
  return _str;
}

FrobbyStringStream::operator const string&() const {
  return _str;
}

void FrobbyStringStream::appendIntegerToString(string& str,
											   unsigned long integer) {
  unsigned long initialLength = str.size();

  // Append string representation of integer with digits in reverse
  // order.
  do {
	unsigned long quotient = integer / 10;
	unsigned long remainder = integer - quotient * 10; // faster than %

	char digit = static_cast<char>(remainder + '0');
	str += digit;

	integer = quotient;

	// condition at end so that zero maps to "0" rather than "".
  } while (integer != 0);

  // Reverse the digits (and only the digits) to get the correct
  // order.
  reverse(str.begin() + initialLength, str.end());
}

void FrobbyStringStream::appendIntegerToString(string& str,
											   const mpz_class& integer) {
  str += integer.get_str();
}

void FrobbyStringStream::parseInteger(mpz_class& integer, const string& str) {
  if (integer.set_str(str, 10) != 0)
	throw NotAnIntegerException
	  ("Argument to FrobbyStringStream::parseInteger not a valid integer.");
}

FrobbyStringStream::NotAnIntegerException::NotAnIntegerException
(const string& str):
  runtime_error(str) {
}
