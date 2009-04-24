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

#ifndef MY_ASSERTS_GUARD
#define MY_ASSERTS_GUARD

#include "MyIdeal.h"
#include "MyPolynomial.h"

// Print an error and exit if a is not equal to b. Otherwise print a
// dot to signify that a test passed.
void assertEqual(const MyIdeal& a, const MyIdeal& b, const char* msg);
void assertEqual(const MyIdeals& a, const MyIdeals& b, const char* msg);

// Print an error and exit if a is not equal to b. Otherwise print a
// dot to signify that a test passed.
void assertEqual(const MyPolynomial& a, const MyPolynomial& b,
				 const char* msg);

// Print an error and exit if b is not true. Otherwise print a dot to
// signify that a test passed.
void assertTrue(bool b, const char* msg);

#endif
