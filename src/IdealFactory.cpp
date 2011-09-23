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
#include "IdealFactory.h"

#include <algorithm>

VarNames IdealFactory::ring_xyzt() {
  VarNames names;
  names.addVar("x");
  names.addVar("y");
  names.addVar("z");
  names.addVar("t");
  return names;
}

BigIdeal IdealFactory::xx_yy_xz_yz() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(2, 0, 0, 0));
  ideal.insert(makeTerm(0, 2, 0, 0));
  ideal.insert(makeTerm(1, 0, 1, 0));
  ideal.insert(makeTerm(0, 1, 1, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::x_y() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(1, 0, 0, 0));
  ideal.insert(makeTerm(0, 1, 0, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::x_y_z() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(1, 0, 0, 0));
  ideal.insert(makeTerm(0, 1, 0, 0));
  ideal.insert(makeTerm(0, 0, 1, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::xyzzz_xxyy() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(1, 1, 3, 0));
  ideal.insert(makeTerm(2, 2, 0, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::xyz_xxyy() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(1, 1, 1, 0));
  ideal.insert(makeTerm(2, 2, 0, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::xx_yy_z() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(2, 0, 0, 0));
  ideal.insert(makeTerm(0, 2, 0, 0));
  ideal.insert(makeTerm(0, 0, 1, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::xy_xxyyz() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(1, 1, 0, 0));
  ideal.insert(makeTerm(2, 2, 1, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::xx_yy_zz_t_xz_yz() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(2, 0, 0, 0));
  ideal.insert(makeTerm(0, 2, 0, 0));
  ideal.insert(makeTerm(0, 0, 2, 0));
  ideal.insert(makeTerm(0, 0, 0, 1));
  ideal.insert(makeTerm(1, 0, 1, 0));
  ideal.insert(makeTerm(0, 1, 1, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::xy_z() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(1, 1, 0, 0));
  ideal.insert(makeTerm(0, 0, 1, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::xy_xz() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(1, 1, 0, 0));
  ideal.insert(makeTerm(1, 0, 1, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::xy() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(1, 1, 0, 0));
  ideal.sortGenerators();
  return ideal;
}

BigIdeal IdealFactory::z() {
  BigIdeal ideal(ring_xyzt());
  ideal.insert(makeTerm(0, 0, 1, 0));
  ideal.sortGenerators();
  return ideal;
}

vector<BigIdeal> IdealFactory::irrdecom_xx_yy_xz_yz() {
  vector<BigIdeal> ideals;
  ideals.push_back(x_y());
  ideals.push_back(xx_yy_z());
  sort(ideals.begin(), ideals.end());
  return ideals;
}

vector<BigIdeal> IdealFactory::assoprimes_xx_yy_xz_yz() {
  vector<BigIdeal> ideals;
  ideals.push_back(x_y());
  ideals.push_back(x_y_z());
  sort(ideals.begin(), ideals.end());
  return ideals;
}

BigIdeal IdealFactory::wholeRing(size_t varCount) {
  BigIdeal ideal((VarNames(varCount)));
  ideal.insert(vector<mpz_class>(varCount));
  return ideal;
}

BigIdeal IdealFactory::zeroIdeal(size_t varCount) {
  return BigIdeal(VarNames(varCount));
}

vector<mpz_class> IdealFactory::makeTerm(int a, int b, int c, int d) {
  vector<mpz_class> t(4);
  t[0] = a;
  t[1] = b;
  t[2] = c;
  t[3] = d;
  return t;
}

vector<mpz_class> IdealFactory::makeTerm(int a) {
  vector<mpz_class> t(1);
  t[0] = a;
  return t;
}
