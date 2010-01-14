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
#include "stdinc.h"
#include "PolynomialFactory.h"

#include "IdealFactory.h"

BigPolynomial PolynomialFactory::hilbert_xx_yy_xz_yz() {
  BigPolynomial poly(IdealFactory::ring_xyzt());
  poly.add(-1, IdealFactory::makeTerm(2, 2, 1, 0));
  poly.add(-1, IdealFactory::makeTerm(0, 1, 1, 0));
  poly.add(-1, IdealFactory::makeTerm(0, 2, 0, 0));
  poly.add(+1, IdealFactory::makeTerm(0, 2, 1, 0));
  poly.add(-1, IdealFactory::makeTerm(2, 0, 0, 0));
  poly.add(+1, IdealFactory::makeTerm(2, 0, 1, 0));
  poly.add(-1, IdealFactory::makeTerm(1, 0, 1, 0));
  poly.add(+1, IdealFactory::makeTerm(1, 1, 1, 0));
  poly.add(+1, IdealFactory::makeTerm(0, 0, 0, 0));
  poly.add(+1, IdealFactory::makeTerm(2, 2, 0, 0));
  poly.sortTermsReverseLex();
  return poly;
}

BigPolynomial PolynomialFactory::one_minus4tt_3ttt_tttt_minusttttt() {
  VarNames names;
  names.addVar("t");
  BigPolynomial poly(names);
  poly.add(+1, IdealFactory::makeTerm(0));
  poly.add(-4, IdealFactory::makeTerm(2));
  poly.add(+3, IdealFactory::makeTerm(3));
  poly.add(+1, IdealFactory::makeTerm(4));
  poly.add(-1, IdealFactory::makeTerm(5));
  poly.sortTermsReverseLex();
  return poly;
}

BigPolynomial PolynomialFactory::one(size_t varCount) {
  BigPolynomial poly((VarNames(varCount)));
  poly.add(1, vector<mpz_class>(varCount));
  return poly;
}

BigPolynomial PolynomialFactory::one(const VarNames& names) {
  BigPolynomial poly(names);
  poly.add(1, vector<mpz_class>(names.getVarCount()));
  return poly;
}

BigPolynomial PolynomialFactory::zero(size_t varCount) {
  return BigPolynomial((VarNames(varCount)));
}
