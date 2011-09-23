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
#ifndef POLYNOMIAL_FACTORY_GUARD
#define POLYNOMIAL_FACTORY_GUARD

#include "BigPolynomial.h"

/** This class has static methods that return various
 polynomials. This provides a convenient way to obtain polynomials for
 tests. The data returned by this class is all in a canonical form
 suitable for comparison using operator==.
*/
class PolynomialFactory {
 public:
  /** Returns the multigraded Hilbert-Poincare series numerator of the
   ideal <x^2, y^2, xz, yz>.
  */
  static BigPolynomial hilbert_xx_yy_xz_yz();

  /** Returns the polynomial 1 - 4t^2 + 3t^3 + t^4 - t^5. */
  static BigPolynomial one_minus4tt_3ttt_tttt_minusttttt();

  /** Returns the polynomial 1 in the ring VarNames(varCount). */
  static BigPolynomial one(size_t varCount);

  /** Returns the polynomial 1 in the ring names. */
  static BigPolynomial one(const VarNames& names);

  /** Return the zero polynomial in the ring VarNames(varCount). */
  static BigPolynomial zero(size_t varCount);
};

#endif
