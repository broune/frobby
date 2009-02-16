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

#ifndef MY_POLYNOMIAL_CREATORS_GUARD
#define MY_POLYNOMIAL_CREATORS_GUARD

#include "MyPolynomial.h"

// This file contains methods that create polynomials of type MyPolynomial.

// Return the polynomial in four variables that is the multigraded
// Hilbert function of the ideal generated by x^2, y^2, xz and yz.
MyPolynomial makeHilbert4_xx_yy_xz_yz();

MyPolynomial makePoly1_1_m4to2_3to3_1to4_m1to5();

// Return the polynomial 1 in a ring of varCount variables.
MyPolynomial makePoly_1(size_t varCount);

// Return the zero polynomial in a ring of some unspecified number of
// variables.
MyPolynomial makePoly_0();

#endif
