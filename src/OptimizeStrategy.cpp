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

  _simplify_dominator(grader.getVarCount()),
  _simplify_oldDominator(grader.getVarCount()),
  _improvement(grader.getVarCount()) {

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
  MsmStrategy::getPivot(pivot, slice, _grader);
}

void OptimizeStrategy::simplify(Slice& slice) {
  ASSERT(slice.getVarCount() == _grader.getVarCount());
  if (slice.getIdeal().getGeneratorCount() == 0)
	return;

  if (!_useBound || _maxSolutions.isZeroIdeal()) {
	slice.simplify();
	return;
  }

  Term& dominator = _simplify_dominator;
  Term& oldDominator = _simplify_oldDominator;

  ASSERT(dominator.getVarCount() == slice.getVarCount());
  ASSERT(oldDominator.getVarCount() == slice.getVarCount());

  if (!getDominator(slice, dominator))
	return;

  while (true) {
	// Obtain upper bound on the degree of elements of msm(I).
	mpz_class& degree = _simplify_degree;
	_grader.getUpperBound(slice.getMultiply(), dominator, degree);

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
	  return;
	}

	oldDominator = dominator;

	if (boundSimplify(slice, dominator, degree)) {
	  if (!getDominator(slice, dominator))
		return; // Slice is now a basecase.
	  if (dominator != oldDominator)
		continue; // Iterate using new dominator.	  
	}

	// Simplify the slice in the usual non-bound way.
	slice.simplify();
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
 const mpz_class& degree) {

  if (getInnerSimplify
	  (slice.getMultiply(), dominator, degree, _improvement))
	slice.innerSlice(_improvement);
  else if (getOuterSimplify
		   (slice.getMultiply(), dominator, degree, _improvement))
	slice.outerSlice(_improvement);
  else
	return false;

  return true;
}

bool OptimizeStrategy::getInnerSimplify
(const Term& divisor,
 const Term& dominator,
 const mpz_class& degree,
 Term& pivot) {

  bool simplifiedAny = false;
  for (size_t var = 0; var < getVarCount(); ++var) {
	ASSERT(_grader.getGrade(var, 0) ==
		   _grader.getGrade(var, _grader.getMaxExponent(var)));

	int sign = _grader.getGradeSign(var);
	if (sign > 0) {
	  mpz_class maxExponent = _maxValue - degree;
	  maxExponent += _grader.getGrade(var, dominator[var]);

	  Exponent improve;
	  bool canSimplify = _grader.getMaxIndexLessThan
		(var, divisor[var], dominator[var] - 1, improve,
		 maxExponent, _reportAllSolutions);

	  if (canSimplify) {
		simplifiedAny = true;
		pivot[var] = improve - divisor[var] + 1;
		continue;
	  }
	} else if (sign < 0 &&
			   dominator[var] == _grader.getMaxExponent(var) &&
			   divisor[var] < dominator[var]) {
	  mpz_class maxExponent = degree - _grader.getGrade(var, dominator[var]);
	  maxExponent += _grader.getGrade(var, divisor[var]);

	  if (_reportAllSolutions ?
		  maxExponent < _maxValue : maxExponent <= _maxValue) {
		simplifiedAny = true;
		pivot[var] = dominator[var] - divisor[var];
		continue;
	  }
	}

	pivot[var] = 0;
  }

  ASSERT(simplifiedAny == !pivot.isIdentity());
  return simplifiedAny;
}

bool OptimizeStrategy::getOuterSimplify
(const Term& divisor,
 const Term& dominator,
 const mpz_class& degree,
 Term& pivot) {

  for (size_t var = 0; var < getVarCount(); ++var) {
	int sign = _grader.getGradeSign(var);
	if (sign < 0) {
	  mpz_class maxExponent = _maxValue - degree;
	  maxExponent += _grader.getGrade(var, divisor[var]);
	  
	  Exponent improve;
	  bool canImprove = _grader.getMinIndexLessThan
		(var, divisor[var] + 1, dominator[var], improve,
		 maxExponent, _reportAllSolutions);
	  
	  if (canImprove) {
		pivot.setToIdentity();
		pivot[var] = improve - divisor[var];
		ASSERT(pivot[var] > 0);
		
		return true;
	  }
	} else if (sign > 0 &&
			   dominator[var] == _grader.getMaxExponent(var) &&
			   divisor[var] < dominator[var]) {
	  mpz_class maxExponent = degree - _grader.getGrade(var, dominator[var] - 1);
	  maxExponent += _grader.getGrade(var, dominator[var]);
	  bool canImprove;
	  if (_reportAllSolutions)
		canImprove = maxExponent < _maxValue;
	  else
		canImprove = maxExponent <= _maxValue;
	  if (canImprove) {
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
  return _improvement.getVarCount();
}
