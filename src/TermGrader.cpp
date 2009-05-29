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
#include "TermGrader.h"

#include "Projection.h"
#include "TermTranslator.h"
#include "Term.h"

TermGrader::TermGrader(const vector<mpz_class>& varDegrees,
					   const TermTranslator& translator):
  _grades(varDegrees.size()) {
  
  // Set up _signs.
  _signs.resize(varDegrees.size());
  for (size_t var = 0; var < varDegrees.size(); ++var) {
	if (varDegrees[var] > 0)
	  _signs[var] = 1;
	else if (varDegrees[var] < 0)
	  _signs[var] = -1;
  }

  // Set up _grades.
  for (size_t var = 0; var < varDegrees.size(); ++var) {
    size_t maxId = translator.getMaxId(var);
    _grades[var].resize(maxId + 1);
    
    for (Exponent e = 0; e <= maxId; ++e)
      _grades[var][e] = varDegrees[var] * translator.getExponent(var, e);
  }
}

mpz_class TermGrader::getDegree(const Term& term) const {
  mpz_class degree;
  getDegree(term, degree);
  return degree;
}

void TermGrader::getDegree(const Term& term, mpz_class& degree) const {
  ASSERT(term.getVarCount() == _grades.size());
  degree = 0;
  for (size_t var = 0; var < term.getVarCount(); ++var)
    degree += getGrade(var, term[var]);
}

void TermGrader::getDegree(const Term& term,
						   const Projection& projection,
						   mpz_class& degree) const {
  ASSERT(term.getVarCount() == projection.getRangeVarCount());
  degree = 0;
  for (size_t var = 0; var < term.getVarCount(); ++var)
    degree += getGrade(projection.inverseProjectVar(var), term[var]);
}

Exponent TermGrader::getLargestLessThan
(size_t var, const mpz_class& value, bool strict) const {
  ASSERT(var < getVarCount());
  ASSERT(!_grades[var].empty());

  if (getGradeSign(var) > 0) {
    if (_grades[var][0] >= value)
	  return 0;

	size_t size = _grades[var].size();
	for (size_t e = 1; e < size; ++e) {
	  const mpz_class& exp = _grades[var][e];
	  if (exp >= value) {
		if (strict || exp != value)
		  return e - 1;
		else
		  return e;
	  }
	}

	return size - 1;
  } 

  if (getGradeSign(var) < 0) {
	ASSERT(false);
  }

  return 0;
}

Exponent TermGrader::getLargestLessThan(size_t var, Exponent from, Exponent to,
										const mpz_class& value, bool strict) const {
  ASSERT(from <= to);

  // We are expecting that the correct value will usually be close to
  // from, so we start with an exponential search starting at from and
  // then move to a binary search when the endpoints become close.

  Exponent low = from;
  Exponent high = to;

  // We carry on as though strict is true, and adjust the value
  // below. The invariant is that degree(low) < value <= degree(high +
  // 1), if that is true to begin with. You can check that both the
  // cases value <= degree(from) and degree(high + 1) < value work out
  // also.
  while (true) {
	ASSERT(low <= high);
	size_t gap = high - low;
	if (gap == 0)
	  break;

	Exponent lowDelta = low - from;

	// pivot is the point we do binary or exponential search on.
	Exponent pivot;
	if (lowDelta < gap) {
	  // In this case we have not moved much from the lower endpoint,
	  // so we double the distance, and add one in case lowDelta is
	  // zero.
	  pivot = low + lowDelta + 1;
	} else {
	  // We use binary search. This formula sets pivot to be the
	  // average of low and high rounded up, while avoiding the
	  // possible overflow inherent in adding low and high.
	  pivot = low + (gap + 1) / 2; 
	}
	ASSERT(low < pivot);
	ASSERT(pivot <= high);

	if (getGrade(var, pivot) < value)
	  low = pivot;
	else
	  high = pivot - 1;
  }
  ASSERT(low == high);

  if (!strict && low < to && getGrade(var, low + 1) == value)
	++low;

  ASSERT((low == from && getLargestLessThan(var, value, strict) <= from) ||
		 (low == to && getLargestLessThan(var, value, strict) >= to) ||
		 (low == getLargestLessThan(var, value, strict)));

  return low;
}

void TermGrader::getIncrementedDegree(const Term& term,
									  const Projection& projection,
									  mpz_class& degree) const {
  ASSERT(term.getVarCount() == projection.getRangeVarCount());
  degree = 0;
  for (size_t var = 0; var < term.getVarCount(); ++var)
    degree += getGrade(projection.inverseProjectVar(var), term[var] + 1);
}

const mpz_class& TermGrader::getGrade(size_t var, Exponent exponent) const {
  ASSERT(var < _grades.size());
  ASSERT(exponent < _grades[var].size());

  return _grades[var][exponent];
}

Exponent TermGrader::getMaxExponent(size_t var) const {
  ASSERT(!_grades[var].empty());
  return _grades[var].size() - 1;
}

size_t TermGrader::getVarCount() const {
  return _grades.size();
}

void TermGrader::print(ostream& out) const {
  out << "TermGrader (\n";
  for (size_t var = 0; var < _grades.size(); ++var) {
	out << " var " << var << ':';
    for (size_t e = 0; e < _grades[var].size(); ++e)
	  out << ' ' << _grades[var][e];
	out << '\n';
  }
  out << ")\n";
}

int TermGrader::getGradeSign(size_t var) const {
  ASSERT(var < _grades.size());
  return _signs[var];
}

ostream& operator<<(ostream& out, const TermGrader& grader) {
  grader.print(out);
  return out;
}
