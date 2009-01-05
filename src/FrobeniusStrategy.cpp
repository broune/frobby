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
#include "FrobeniusStrategy.h"

#include "TermGrader.h"
#include "Slice.h"

FrobeniusStrategy::FrobeniusStrategy(TermGrader& grader,
									 const SplitStrategy* splitStrategy,
									 bool reportAllSolutions,
									 bool useBound):
  MsmStrategy(this, splitStrategy),
  _grader(grader),
  _maxSolutions(grader.getVarCount()),
  _reportAllSolutions(reportAllSolutions),
  _useBound(useBound),

  _simplify_bound(grader.getVarCount()),
  _simplify_oldBound(grader.getVarCount()),
  _simplify_colon(grader.getVarCount()) {
}

const Ideal& FrobeniusStrategy::getMaximalSolutions() {
  return _maxSolutions;
}

const mpz_class& FrobeniusStrategy::getMaximalValue() {
  ASSERT(!_maxSolutions.isZeroIdeal());
  return _maxValue;
}

void FrobeniusStrategy::beginConsuming() {
  _maxSolutions.clear();
}

void FrobeniusStrategy::consume(const Term& term) {
  mpz_class& degree = _consume_degree;

  _grader.getDegree(term, degree);
  if (_maxSolutions.isZeroIdeal() || degree > _maxValue) {
	_maxValue = degree;
	_maxSolutions.clear();
	_maxSolutions.insert(term);
  } else if (_reportAllSolutions && degree == _maxValue)
	_maxSolutions.insert(term);
}

void FrobeniusStrategy::doneConsuming() {
}

void FrobeniusStrategy::getPivot(Term& pivot, Slice& slice) {
  if (!_split->isFrobeniusSplit()) {
	MsmStrategy::getPivot(pivot, slice);
	return;
  }

  const Term& lcm = slice.getLcm();

  mpz_class& maxDiff = _getPivot_maxDiff;
  mpz_class& diff = _getPivot_diff;

  maxDiff = -1;
  size_t maxOffset = (size_t)-1;
  for (size_t var = 0; var < slice.getVarCount(); ++var) {
	if (lcm[var] <= 1)
	  continue;

	Exponent e = lcm[var] / 2;
	diff =
	  _grader.getGrade(var, slice.getMultiply()[var] + e) -
	  _grader.getGrade(var, slice.getMultiply()[var]);
	ASSERT(diff >= 0);

	if (diff > maxDiff) {
	  maxOffset = var;
	  maxDiff = diff;
	}
  }
  ASSERT(maxOffset != (size_t)-1);

  pivot.setToIdentity();
  pivot[maxOffset] = lcm[maxOffset] / 2;
}

void FrobeniusStrategy::simplify(Slice& slice) {
  ASSERT(slice.getVarCount() == _grader.getVarCount());
  if (slice.getIdeal().getGeneratorCount() == 0)
	return;

  if (!_maxSolutions.isZeroIdeal() || !_useBound) {
	slice.simplify();
	return;
  }

  Term& bound = _simplify_bound;
  Term& oldBound = _simplify_oldBound;
  Term& colon = _simplify_colon;

  ASSERT(bound.getVarCount() == slice.getVarCount());
  ASSERT(oldBound.getVarCount() == slice.getVarCount());
  ASSERT(colon.getVarCount() == slice.getVarCount());

  getUpperBound(slice, bound);

  while (true) {
	// Obtain bound for degree
	mpz_class& degree = _simplify_degree;
	_grader.getDegree(bound, degree);

	// Check if improvement is possible
	bool interesting = true;
	if (_reportAllSolutions)
	  interesting = (degree >= _maxValue);
	else
	  interesting = (degree > _maxValue);
	if (!interesting) {
	  slice.clearIdealAndSubtract();
	  break;
	}

	// Use above bound to obtain improved lower bound. The idea is
	// to consider artinian pivots and to rule out the outer slice
	// using the above condition. If this can be done, then we can
	// perform the split and ignore the outer slice.
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  colon[var] =
		improveLowerBound(var, degree, bound, slice.getMultiply());

	// Check if any improvement were made.
	oldBound = bound;
	if (!colon.isIdentity()) {
	  slice.innerSlice(colon);
	  getUpperBound(slice, bound);
	  if (bound != oldBound)
		continue; // Iterate process using new bound.
	}

	slice.simplify();
	if (slice.getIdeal().getGeneratorCount() == 0)
	  break;

	getUpperBound(slice, bound);
	if (bound == oldBound)
	  break;

	// Iterate process using new bound.
  }
}

Exponent FrobeniusStrategy::
improveLowerBound(size_t var,
				  const mpz_class& upperBoundDegree,
				  const Term& upperBound,
				  const Term& lowerBound) {
  if (upperBound[var] == lowerBound[var])
	return 0;

  mpz_class& baseUpperBoundDegree = _improveLowerBound_baseUpperBoundDegree;
  baseUpperBoundDegree = upperBoundDegree -
	_grader.getGrade(var, upperBound[var]);

  // Exponential search followed by binary search. We cannot just
  // simply use a formula since the grader can use a translator that
  // is just a table with no mathematical signifiance.
  Exponent low = 0;
  Exponent high = upperBound[var] - lowerBound[var];

  // The invariant here is that low <= high and high - low decreases
  // at each iteration. Also, bound(mid - 1) <= _maxValue <
  // bound(high).
  while (low < high) {
	Exponent mid;
	if (low < high - low)
	  mid = 2 * low; // Using exponential search.
	else {
	  // Using binary search. Note that this way of expressing (low +
	  // high) / 2 avoids the possibility of low + high causing an
	  // overflow.
	  mid = low + (high - low) / 2;
	}

	mpz_class& value = _improveLowerBound_value;
	value = baseUpperBoundDegree +
	  _grader.getGrade(var, lowerBound[var] + mid);

	if (value <= _maxValue)
	  low = mid + 1;
	else
	  high = mid;
  }
  ASSERT(low == high);
  ASSERT(low >= 1);

  // If we just want one solution, we can require strict improvement
  // when looking at new solutions. When we want all, then we can only
  // require equal to or better, so we have to decrease the bound by
  // one.
  if (_reportAllSolutions)
	--low;

  return low;
}

void FrobeniusStrategy::getUpperBound(const Slice& slice, Term& bound) {
  ASSERT(bound.getVarCount() == slice.getVarCount());
  bound = slice.getLcm();

  bound.product(bound, slice.getMultiply());

  for (size_t var = 0; var < bound.getVarCount(); ++var) {
	ASSERT(bound[var] > 0);
	--bound[var];
  }

  for (size_t var = 0; var < bound.getVarCount(); ++var)
	if (bound[var] == _grader.getMaxExponent(var) &&
		slice.getMultiply()[var] < bound[var])
	  --bound[var];
}
