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
#ifndef MY_IDEAL_FACTORY
#define MY_IDEAL_FACTORY

#include "BigIdeal.h"
#include "VarNames.h"

#include <vector>

/** This class has static methods that return various ideals. This
 provides a convenient way to obtain ideals for tests. The data
 returned by this class is all in a canonical form suitable for
 comparison using operator==.
*/
class IdealFactory {
 public:
  /** Returns the ring in the four variables x, y, z and t. This ring
   is used for the ideals returned by the methods of this class unless
   otherwise noted. */
  static VarNames ring_xyzt();

  /** Returns <x^2, y^2, x*z, y*z>. */
  static BigIdeal xx_yy_xz_yz();

  /** Returns <x, y>. */
  static BigIdeal x_y();

  /** Returns <x*y*z^3, x^2y^2>. */
  static BigIdeal xyzzz_xxyy();

  /** Returns <x*y*z, x^2, y^2>. */
  static BigIdeal xyz_xxyy();

  /** Returns <x^2, y^2, z>. */
  static BigIdeal xx_yy_z();

  /** Returns <x*y, x^2*y^2*z>. */
  static BigIdeal xy_xxyyz();

  /** Returns <x^2, y^2, z^2, t, x*z, y*z>. */
  static BigIdeal xx_yy_zz_t_xz_yz();

  /** Returns <x*y, z>. */
  static BigIdeal xy_z();

  /** Returns <x*y>. */
  static BigIdeal xy();

  /** Returns <z>. */
  static BigIdeal z();

  /** Returns <1> in the ring VarNames(varCount). */
  static BigIdeal wholeRing(size_t varCount);

  /** Returns the ideal <0> in the ring VarNames(varCount). */
  static BigIdeal zeroIdeal(size_t varCount);

  /** Returns the irreducible decomposition of <x^2, y^2, x*z, y*z>. */
  static vector<BigIdeal> irrdecom_xx_yy_xz_yz();

  /** Returns a vector representing x^(a,b,c,d). */
  static vector<mpz_class> makeTerm(int a, int b, int c, int d);

  /** Returns a vector representing x^(a). */
  static vector<mpz_class> makeTerm(int a);
};

#endif
