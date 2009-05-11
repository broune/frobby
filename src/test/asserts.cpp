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
#include "asserts.h"

#include <sstream>

AssertException::AssertException(const string& str):
  logic_error(str) {
}

void assertFailed(const char* errorMsg,
				  const char* testName, const char* file, size_t line) {
  if (testName == 0)
	testName = "";

  stringstream msg;
  msg << "Unit test " << testName
	  << " failed in file " << file
	  << " on line " << line << ".\n"
	  << errorMsg;
  throw AssertException(msg.str());
}

void assertTrue(bool value, const char* valueString,
				const char* testName, const char* file, size_t line) {
  if (value)
	return;

  stringstream msg;
  msg << "Expected \n   " << valueString << "\nto be true, but it was not.\n";
  assertFailed(msg.str().c_str(), testName, file, line);
}

void assertEqualFailed(const char* a, const char* b,
					   const char* aString, const char* bString,
					   const char* testName, const char* file, size_t line) {
  stringstream msg;
  msg << "Expected " << aString << " == " << bString << ", but\n"
	  << "the  left hand side was equal to " << a << ", while\n"
	  << "the right hand side was equal to " << b << ".\n";
  assertFailed(msg.str().c_str(), testName, file, line);
}
