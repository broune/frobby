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
#include "Polynomial.h"

Polynomial::Polynomial(size_t varCount):
  _varCount(varCount) {
}

void Polynomial::add(const mpz_class& coef, const Term& term) {
  ASSERT(_varCount == term.getVarCount());

  if (coef == 0)
	return;

  _terms.resize(_terms.size() + 1);
  _terms.back().coef = coef;
  _terms.back().term = term;
}

void Polynomial::sortLex() {
  sort(_terms.begin(), _terms.end());
}

bool Polynomial::CoefTerm::operator<(const CoefTerm& coefTerm) const {
  return coefTerm.term < term;
}
