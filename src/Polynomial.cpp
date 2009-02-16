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

#include <algorithm>

Polynomial::Polynomial():
  _varCount(0) {
}

Polynomial::Polynomial(size_t varCount):
  _varCount(varCount) {
}

size_t Polynomial::getVarCount() const {
  return _varCount;
}

size_t Polynomial::getTermCount() const {
  return _terms.size();
}

const mpz_class& Polynomial::getCoef(size_t index) const {
  ASSERT(index < getTermCount());
  return _terms[index].coef;
}

const Term& Polynomial::getTerm(size_t index) const {
  ASSERT(index < getTermCount());
  return _terms[index].term;
}

void Polynomial::clearAndSetVarCount(size_t varCount) {
  clear();
  _varCount = varCount;
}

void Polynomial::add(const mpz_class& coef, const Term& term) {
  ASSERT(_varCount == term.getVarCount());

  if (coef == 0)
	return;

  _terms.resize(_terms.size() + 1);
  try {
	_terms.back().coef = coef;
	_terms.back().term = term;
  } catch (std::bad_alloc) {
	_terms.pop_back();
	throw;
  }
}

void Polynomial::sortTermsReverseLex(bool collect) {
  if (_terms.empty())
	return;

  sort(_terms.begin(), _terms.end());

  if (!collect)
	return;

  // TODO: improve collection. E.g. have it be its own method.

  size_t last = 0;
  for (size_t i = 1; i < _terms.size(); ++i) {
	if (_terms[last].term == _terms[i].term)
	  _terms[last].coef += _terms[last].coef;
	else {
	  if (_terms[last].coef == 0)
		_terms[last] = _terms[i];
	  else {
		++last;
		if (last != i)
		  _terms[last] = _terms[i];
	  }
	}
  }

  ASSERT(last < _terms.size());
  _terms.erase(_terms.begin() + last + 1, _terms.end());	
}

bool Polynomial::CoefTerm::operator<(const CoefTerm& coefTerm) const {
  ASSERT(term.getVarCount() == coefTerm.term.getVarCount());
  return ::reverseLexCompare(term, coefTerm.term, term.getVarCount()) < 0;
}

void Polynomial::clear() {
  _terms.clear();
}
