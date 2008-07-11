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

#ifndef MY_IDEAL_CREATORS_GUARD
#define MY_IDEAL_CREATORS_GUARD

#include "MyIdeal.h"

// This file contains methods that create ideals of type MyIdeal.

// Return the ideal in four variables generated by x^2, y^2, xz and yz.
MyIdeal make4_xx_yy_xz_yz();

// Return the ideal in four variables generated by x and y.
MyIdeal make4_x_y();

// Return the ideal in four variables generated by xyz^3 and x^2y^2.
MyIdeal make4_xyzzz_xxyy();

// Return the ideal in four variables generated by xyz and x^2y^2.
MyIdeal make4_xyz_xxyy();

// Return the ideal in 4 variables generated by x^2, y^2 and z.
MyIdeal make4_xx_yy_z();

MyIdeal make4_xy_xxyyz();

MyIdeal make4_xx_yy_zz_t_xz_yz();

MyIdeal make4_xy_z();

MyIdeal make4_xy();

MyIdeal make4_z();

// Return the irreducible decomposition of the ideal.
MyIdeals makeIrrdecom4_xx_yy_xz_yz();

// Return the ideal in varCount variables generated by 1.
MyIdeal make_1(size_t varCount);

// Return the zero ideal in some unspecified number of variables.
MyIdeal make_0();

#endif
