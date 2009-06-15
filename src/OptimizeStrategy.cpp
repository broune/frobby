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

  _simplify_tmpDominator(grader.getVarCount()),
  _simplify_tmpOldDominator(grader.getVarCount()),
  _boundSimplify_tmpPivot(grader.getVarCount()) {

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
  mpz_class& degree = _consume_tmpDegree;

  _grader.getDegree(term, degree);
  if (_maxSolutions.isZeroIdeal() || degree > _maxValueToBeat) {
	if (_reportAllSolutions && degree == _maxValue)
	  _maxSolutions.insert(term);
	else {
	  _maxValue = degree;
	  _maxValueToBeat = degree - _reportAllSolutions;
	  _maxSolutions.clear();
	  _maxSolutions.insert(term);
	}
  }
}

void OptimizeStrategy::doneConsuming() {
}

void OptimizeStrategy::getPivot(Term& pivot, Slice& slice) {
  MsmStrategy::getPivot(pivot, slice, _grader);
}

void OptimizeStrategy::simplify(Slice& slice) {
  ASSERT(slice.getVarCount() == getVarCount());
  if (slice.getIdeal().getGeneratorCount() == 0)
	return;

  if (!_useBound || _maxSolutions.isZeroIdeal()) {
	MsmStrategy::simplify(slice);
	return;
  }

  Term& dominator = _simplify_tmpDominator;
  Term& oldDominator = _simplify_tmpOldDominator;

  ASSERT(dominator.getVarCount() == getVarCount());
  ASSERT(oldDominator.getVarCount() == getVarCount());

  if (!getDominator(slice, dominator))
	return;

  while (true) {
	// Obtain upper bound on the degree of elements of msm(I).
	mpz_class& upperBound = _simplify_tmpUpperBound;
	_grader.getUpperBound(slice.getMultiply(), dominator, upperBound);

	// Check if improvement on the best value found so far is possible
	// from this slice according to the bound. If it is not, then
	// there is no point in looking further at this slice.
	if (upperBound <= _maxValueToBeat) {
	  slice.clearIdealAndSubtract();
	  return;
	}

	oldDominator = dominator;

	if (boundSimplify(slice, dominator, upperBound)) {
	  if (!getDominator(slice, dominator))
		return; // Slice is now a basecase.
	  if (dominator != oldDominator)
		continue; // Iterate using new dominator.	  
	}

	// Simplify the slice in the usual non-bound way.
	MsmStrategy::simplify(slice);
	if (!getDominator(slice, dominator))
	  return; // Slice is now a basecase.
	if (dominator != oldDominator)
	  continue; // Iterate using new dominator.

	break;
  }
}

bool OptimizeStrategy::boundSimplify
(Slice& slice,
 const Term& dominator,
 const mpz_class& upperBound) {

  Term& pivot = _boundSimplify_tmpPivot;

  if (getInnerSimplify(slice.getMultiply(), dominator, upperBound, pivot))
	slice.innerSlice(pivot);
  else if (getOuterSimplify(slice.getMultiply(), dominator, upperBound, pivot))
	slice.outerSlice(pivot);
  else
	return false;

  return true;
}

bool OptimizeStrategy::getInnerSimplify
(const Term& divisor,
 const Term& dominator,
 const mpz_class& upperBound,
 Term& pivot) {

  bool simplifiedAny = false;
  for (size_t var = 0; var < getVarCount(); ++var) {
	ASSERT(_grader.getGrade(var, 0) ==
		   _grader.getGrade(var, _grader.getMaxExponent(var)));
	ASSERT(divisor[var] <= dominator[var]);
	pivot[var] = 0;

	if (divisor[var] == dominator[var])
	  continue;

	int sign = _grader.getGradeSign(var);
	if (sign > 0) {
	  Exponent B;
	  if (dominator[var] != _grader.getMaxExponent(var))
		B = dominator[var];
	  else {
		B = dominator[var] - 1;
		if (divisor[var] == B)
		  continue;
	  }

	  _tmpC = _maxValueToBeat - upperBound;
	  _tmpC += _grader.getGrade(var, B);
		
	  Exponent tPrime;
	  bool foundNonImproving = _grader.getMaxIndexLessThan
		(var, divisor[var], B - 1, tPrime, _tmpC);
	  
	  if (foundNonImproving) {
		simplifiedAny = true;
		pivot[var] = tPrime - divisor[var] + 1;
		ASSERT(pivot[var] > 0);
	  }
	} else if (sign < 0) {
	  if (dominator[var] != _grader.getMaxExponent(var))
		continue;
	  _tmpC = upperBound - _grader.getGrade(var, dominator[var]);
	  _tmpC += _grader.getGrade(var, divisor[var]);

	  if (_tmpC <= _maxValueToBeat) {
		simplifiedAny = true;
		pivot[var] = dominator[var] - divisor[var];
		ASSERT(pivot[var] > 0);
	  }
	}
  }

  ASSERT(simplifiedAny == !pivot.isIdentity());
  return simplifiedAny;
}

bool OptimizeStrategy::getOuterSimplify
(const Term& divisor,
 const Term& dominator,
 const mpz_class& upperBound,
 Term& pivot) {

  for (size_t var = 0; var < getVarCount(); ++var) {
	ASSERT(_grader.getGrade(var, 0) ==
		   _grader.getGrade(var, _grader.getMaxExponent(var)));
	ASSERT(divisor[var] <= dominator[var]);
	if (divisor[var] == dominator[var])
	  continue;

	int sign = _grader.getGradeSign(var);
	if (sign < 0) {
	  if (dominator[var] == _grader.getMaxExponent(var))
		continue;

	  _tmpC = _maxValueToBeat - upperBound;
	  _tmpC += _grader.getGrade(var, divisor[var]);
	  
	  Exponent tPrime;
	  bool foundNonImproving = _grader.getMinIndexLessThan
		(var, divisor[var] + 1, dominator[var], tPrime, _tmpC);

	  if (foundNonImproving) {
		pivot.setToIdentity();
		pivot[var] = tPrime - divisor[var];
		ASSERT(pivot[var] > 0);
		
		return true;
	  }
	} else if (sign > 0) {
	  if (dominator[var] != _grader.getMaxExponent(var))
		continue;

	  _tmpC = upperBound - _grader.getGrade(var, dominator[var] - 1);
	  _tmpC += _grader.getGrade(var, dominator[var]);

	  if (_tmpC <= _maxValueToBeat) {
		pivot.setToIdentity();
		pivot[var] = dominator[var] - divisor[var];
		ASSERT(pivot[var] > 0);

		return true;
	  }
	}
  }

  return false;
}

bool OptimizeStrategy::getDominator(Slice& slice, Term& dominator) {
  ASSERT(dominator.getVarCount() == slice.getVarCount());

  // The dominator is pi(lcm(min I)), where I is the ideal represented
  // by the slice, and pi decrements each exponent by one.

  const Term& multiply = slice.getMultiply();
  const Term& lcm = slice.getLcm();

  for (size_t var = 0; var < dominator.getVarCount(); ++var) {
	if (lcm[var] == 0) {
	  slice.clearIdealAndSubtract();
	  return false;
	}

	dominator[var] = multiply[var] + lcm[var] - 1;
  }

  return true;
}

size_t OptimizeStrategy::getVarCount() const {
  return _grader.getVarCount();
}
