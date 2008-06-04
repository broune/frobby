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
#include "BigPolynomial.h"

#include "Term.h"
#include "TermTranslator.h"

BigPolynomial::BigPolynomial(const VarNames& names):
  _names(names) {
}

size_t BigPolynomial::getTermCount() const {
  return _coefTerms.size();
}

size_t BigPolynomial::getVarCount() const {
  return _names.getVarCount();
}

const VarNames& BigPolynomial::getNames() const {
  return _names;
}

const mpz_class& BigPolynomial::getCoef(size_t index) const {
  ASSERT(index < getTermCount());

  return _coefTerms[index].coef;
}

const vector<mpz_class>& BigPolynomial::getTerm(size_t index) const {
  ASSERT(index < getTermCount());

  return _coefTerms[index].term;
}

void BigPolynomial::add(const mpz_class& coef,
						const vector<mpz_class> term) {
  ASSERT(term.size() == getVarCount());

  _coefTerms.resize(_coefTerms.size() + 1);
  _coefTerms.back().coef = coef;
  _coefTerms.back().term = term;
}

void BigPolynomial::add(const mpz_class& coef, mpz_ptr* term) {
  ASSERT(term != 0);

  _coefTerms.resize(_coefTerms.size() + 1);
  _coefTerms.back().coef = coef;
  _coefTerms.back().term = vector<mpz_class>(term, term + getVarCount());
}

void BigPolynomial::add(const mpz_class& coef,
						const Term& term,
						TermTranslator* translator) {
  ASSERT(term.getVarCount() == getVarCount());
  ASSERT(translator != 0);

  _coefTerms.resize(_coefTerms.size() + 1);
  _coefTerms.back().coef = coef;

  vector<mpz_class>& bigTerm = _coefTerms.back().term;
  bigTerm.reserve(term.getVarCount());
  for (size_t var = 0; var < term.getVarCount(); ++var)
	bigTerm.push_back(translator->getExponent(var, term));
}