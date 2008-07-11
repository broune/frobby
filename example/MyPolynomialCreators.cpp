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

#include "MyPolynomialCreators.h"

MyPolynomial makeHilbert4_xx_yy_xz_yz() {
  MyPolynomial polynomial;
  polynomial.push_back(MyTerm(-1,  2, 2, 1, 0));
  polynomial.push_back(MyTerm(-1,  0, 1, 1, 0));
  polynomial.push_back(MyTerm(-1,  0, 2, 0, 0));
  polynomial.push_back(MyTerm(+1,  0, 2, 1, 0));
  polynomial.push_back(MyTerm(-1,  2, 0, 0, 0));
  polynomial.push_back(MyTerm(+1,  2, 0, 1, 0));
  polynomial.push_back(MyTerm(-1,  1, 0, 1, 0));
  polynomial.push_back(MyTerm(+1,  1, 1, 1, 0));
  polynomial.push_back(MyTerm(+1,  0, 0, 0, 0));
  polynomial.push_back(MyTerm(+1,  2, 2, 0, 0));
  return polynomial;
}

MyPolynomial makePoly1_1_m4to2_3to3_1to4_m1to5() {
  MyPolynomial polynomial;
  polynomial.push_back(MyTerm(+1,  0));
  polynomial.push_back(MyTerm(-4,  2));
  polynomial.push_back(MyTerm(+3,  3));
  polynomial.push_back(MyTerm(+1,  4));
  polynomial.push_back(MyTerm(-1,  5));
  return polynomial;
}

MyPolynomial makePoly_1(size_t varCount) {
  MyPolynomial polynomial;
  polynomial.push_back(MyTerm(1, MyPP(varCount)));
  return polynomial;
}

MyPolynomial makePoly_0() {
  return MyPolynomial();
}
