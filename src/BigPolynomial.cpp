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
#include "VarSorter.h"

#include <algorithm>

BigPolynomial::BigPolynomial() {
}

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

void BigPolynomial::clearAndSetNames(const VarNames& names) {
  clear();
  _names = names;
}

void BigPolynomial::sortTerms() {
  sort(_coefTerms.begin(), _coefTerms.end(), compareCoefTerms);
}

void BigPolynomial::sortVariables() {
  VarSorter sorter(_names);
  sorter.getOrderedNames(_names);
  for (size_t i = 0; i < _coefTerms.size(); ++i)
    sorter.permute(_coefTerms[i].term);
}

void BigPolynomial::clear() {
  _coefTerms.clear();
  _names.clear();
}

const mpz_class& BigPolynomial::getCoef(size_t index) const {
  ASSERT(index < getTermCount());

  return _coefTerms[index].coef;
}

const vector<mpz_class>& BigPolynomial::getTerm(size_t index) const {
  ASSERT(index < getTermCount());

  return _coefTerms[index].term;
}

void BigPolynomial::newLastTerm() {
  _coefTerms.resize(_coefTerms.size() + 1);
  _coefTerms.back().term.resize(getVarCount());
}

vector<mpz_class>& BigPolynomial::getLastTerm() {
  ASSERT(getTermCount() > 0);

  return _coefTerms.back().term;
}

mpz_class& BigPolynomial::getLastCoef() {
  ASSERT(getTermCount() > 0);

  return _coefTerms.back().coef;
}

void BigPolynomial::renameVars(const VarNames& names) {
  ASSERT(names.getVarCount() == _names.getVarCount());
  _names = names;  
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

bool BigPolynomial::compareCoefTerms(const BigCoefTerm& a,
									 const BigCoefTerm& b) {
  for (size_t var = 0; var < a.term.size(); ++var)
	if (a.term[var] != b.term[var])
	  return a.term[var] > b.term[var];
  return a.coef < b.coef;
}
