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

#include "MyIdealCreators.h"

MyIdeal make4_xx_yy_xz_yz() {
  size_t varCount = 4;

  MyPP xx(varCount);
  xx[0] = 2;

  MyPP yy(varCount);
  yy[1] = 2;

  MyPP xz(varCount);
  xz[0] = 1;
  xz[2] = 1;

  MyPP yz(varCount);
  yz[1] = 1;
  yz[2] = 1;

  MyIdeal ideal;
  ideal.push_back(xx);
  ideal.push_back(yy);
  ideal.push_back(xz);
  ideal.push_back(yz);
  return ideal;
}

MyIdeal make4_x_y() {
  size_t varCount = 4;

  MyPP x(varCount);
  x[0] = 1;

  MyPP y(varCount);
  y[1] = 1;

  MyIdeal ideal;
  ideal.push_back(x);
  ideal.push_back(y);
  return ideal;
}

MyIdeal make4_xyzzz_xxyy() {
  size_t varCount = 4;

  MyPP xyzzz(varCount);
  xyzzz[0] = 1;
  xyzzz[1] = 1;
  xyzzz[2] = 3;

  MyPP xxyy(varCount);
  xxyy[0] = 2;
  xxyy[1] = 2;

  MyIdeal ideal;
  ideal.push_back(xyzzz);
  ideal.push_back(xxyy);
  return ideal;
}

MyIdeal make4_xyz_xxyy() {
  size_t varCount = 4;

  MyPP xyz(varCount);
  xyz[0] = 1;
  xyz[1] = 1;
  xyz[2] = 1;

  MyPP xxyy(varCount);
  xxyy[0] = 2;
  xxyy[1] = 2;

  MyIdeal ideal;
  ideal.push_back(xyz);
  ideal.push_back(xxyy);
  return ideal;
}

MyIdeal make4_xx_yy_z() {
  size_t varCount = 4;

  MyPP xx(varCount);
  xx[0] = 2;

  MyPP yy(varCount);
  yy[1] = 2;

  MyPP z(varCount);
  z[2] = 1;

  MyIdeal ideal;
  ideal.push_back(xx);
  ideal.push_back(yy);
  ideal.push_back(z);

  return ideal;
}

MyIdeal make4_xy_xxyyz() {
  MyIdeal ideal;
  ideal.push_back(makePP(1, 1, 0, 0));
  ideal.push_back(makePP(2, 2, 1, 0));
  return ideal;
}

MyIdeal make4_xx_yy_zz_t_xz_yz() {
  MyIdeal ideal;
  ideal.push_back(makePP(2, 0, 0, 0));
  ideal.push_back(makePP(0, 2, 0, 0));
  ideal.push_back(makePP(0, 0, 2, 0));
  ideal.push_back(makePP(0, 0, 0, 1));
  ideal.push_back(makePP(1, 0, 1, 0));
  ideal.push_back(makePP(0, 1, 1, 0));
  return ideal;
}

MyIdeal make4_xy_z() {
  MyIdeal ideal;
  ideal.push_back(makePP(1, 1, 0, 0));
  ideal.push_back(makePP(0, 0, 1, 0));
  return ideal;
}

MyIdeal make4_xy() {
  MyIdeal ideal;
  ideal.push_back(makePP(1, 1, 0, 0));
  return ideal;
}

MyIdeal make4_z() {
  MyIdeal ideal;
  ideal.push_back(makePP(0, 0, 1, 0));
  return ideal;
}

MyIdeals makeIrrdecom4_xx_yy_xz_yz() {
  MyIdeals ideals;
  ideals.push_back(make4_x_y());
  ideals.push_back(make4_xx_yy_z());
  return ideals;
}

MyIdeal make_1(size_t varCount) {
  MyPP one(varCount);
  MyIdeal ideal;
  ideal.push_back(one);
  return ideal;
}

MyIdeal make_0() {
  return MyIdeal();
}
