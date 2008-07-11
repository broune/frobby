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

#ifndef MY_POLYNOMIAL_GUARD
#define MY_POLYNOMIAL_GUARD

#include "MyIdeal.h"

// Represents a term with a coefficient.
struct MyTerm {
  MyTerm(int coef, MyPP exponents);
  MyTerm(int coef, int e);
  MyTerm(int coef, int e1, int e2, int e3, int e4);

  bool operator<(const MyTerm& term) const;
  bool operator==(const MyTerm& term) const;

  int coefficient;
  MyPP exponents;
};

// Represents a polynomial.
typedef vector<MyTerm> MyPolynomial;

// Returns true if a is equal to b.
bool equal(MyPolynomial a, MyPolynomial b);

// Print polynomial to standard out.
void printPolynomial(MyPolynomial polynomial);

#endif
