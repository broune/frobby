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
#include "OptimizeStrategy.h"

#include "TermGrader.h"
#include "Slice.h"

OptimizeStrategy::OptimizeStrategy(TermGrader& grader,
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

  MsmStrategy::setUseIndependence(false);
}

const Ideal& OptimizeStrategy::getMaximalSolutions() {
  return _maxSolutions;
}

const mpz_class& OptimizeStrategy::getMaximalValue() {
  ASSERT(!_maxSolutions.isZeroIdeal());
  return _maxValue;
}

void OptimizeStrategy::setUseIndependence(bool use) {
  ASSERT(!use);
}

void OptimizeStrategy::beginConsuming() {
  _maxSolutions.clear();
}

void OptimizeStrategy::consume(const Term& term) {
  mpz_class& degree = _consume_degree;

  _grader.getDegree(term, degree);
  if (_maxSolutions.isZeroIdeal() || degree > _maxValue) {
	_maxValue = degree;
	_maxSolutions.clear();
	_maxSolutions.insert(term);
  } else if (_reportAllSolutions && degree == _maxValue)
	_maxSolutions.insert(term);
}

void OptimizeStrategy::doneConsuming() {
}

void OptimizeStrategy::getPivot(Term& pivot, Slice& slice) {
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

void OptimizeStrategy::simplify(Slice& slice) {
  ASSERT(slice.getVarCount() == _grader.getVarCount());
  if (slice.getIdeal().getGeneratorCount() == 0)
	return;

  if (!_useBound || _maxSolutions.isZeroIdeal()) {
	slice.simplify();
	return;
  }

  Term& bound = _simplify_bound;
  Term& oldBound = _simplify_oldBound;
  Term& colon = _simplify_colon;

  ASSERT(bound.getVarCount() == slice.getVarCount());
  ASSERT(oldBound.getVarCount() == slice.getVarCount());
  ASSERT(colon.getVarCount() == slice.getVarCount());

  getMonomialBound(slice, bound);

  while (true) {
	// Obtain upper bound on the degree of elements of msm(I).
	mpz_class& degree = _simplify_degree;
	_grader.getDegree(bound, degree);

	// Check if improvement on the best value found so far is possible
	// from this slice according to the bound. If it is not, then
	// there is no point in looking further at this slice.
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
	slice.print(stdout);
	bound.print(stdout);
	for (size_t var = 0; var < slice.getVarCount(); ++var) {
	  printf("%d\n", var);
	  fflush(stdout);
	  int sign = _grader.getGradeSign(var);
	  if (sign > 0)
		colon[var] =
		  improveLowerBound(var, degree, bound, slice.getMultiply());
	  // TODO: implement this for sign < 0 too.
	}

	// Check if any improvement was made on the lower bound.
	oldBound = bound;
	if (!colon.isIdentity()) {
	  slice.innerSlice(colon);
	  getMonomialBound(slice, bound);
	  if (bound != oldBound)
		continue; // Iterate process using new bound.
	}

	// Simplify the slice in the usual non-bound way.
	slice.simplify();
	if (slice.getIdeal().getGeneratorCount() == 0)
	  break; // In this case we had simplified the slice to a basecase.

	getMonomialBound(slice, bound);
	if (bound == oldBound)
	  break; // The bound is unchanged, so no further improvement can be made.

	// Iterate process using new bound.
  }
}

Exponent OptimizeStrategy::
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
	  mid = 2 * low + 1; // Using exponential search.
	else {
	  // Using binary search. Note that this way of expressing (low +
	  // high) / 2 avoids the possibility of low + high causing an
	  // overflow.
	  mid = low + (high - low + 1) / 2;
	}

	mpz_class& value = _improveLowerBound_value;
	value = baseUpperBoundDegree +
	  _grader.getGrade(var, lowerBound[var] + mid);

	// If we just want one solution, we can require strict improvement
	// when looking at new solutions. When we want all, then we can
	// only require equal to or better.
	if (value < _maxValue || (!_reportAllSolutions && value == _maxValue))
	  low = mid;
	else
	  high = mid - 1;
  }
  ASSERT(low == high);

  return low;
}

void OptimizeStrategy::getMonomialBound(const Slice& slice, Term& bound) {
  ASSERT(bound.getVarCount() == slice.getVarCount());

  // We are combining an upper and a lower monomial bound, using the
  // upper bound for var if the grading is increasing, and the lower
  // bound if the grading is decreasing. This implies that the degree
  // of this bound will be an upper bound on the degree of any element
  // of msm(I), where I is the ideal represented by the slice.
  //
  // The lower bound is simply slice.getMultiply(), while the upper
  // bound is pi(lcm(min I)), where I is the ideal represented by the
  // slice, and pi decrements each exponent by one.

  for (size_t var = 0; var < bound.getVarCount(); ++var) {
	int sign = _grader.getGradeSign(var);
	if (sign > 0) {
	  bound[var] = slice.getMultiply()[var] + slice.getLcm()[var];
	  if (bound[var] >= 1)
		bound[var] -= 1;
	} else
	  bound[var] = slice.getMultiply()[var];
  }
}
