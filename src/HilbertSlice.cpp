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
#include "HilbertSlice.h"

#include "CoefTermConsumer.h"

HilbertSlice::HilbertSlice():
  _varCount(0) {
}

HilbertSlice::HilbertSlice(const Ideal& ideal, const Ideal& subtract,
			 const Term& multiply):
  _varCount(multiply.getVarCount()),
  _multiply(multiply),
  _lcm(multiply.getVarCount()),
  _lcmUpdated(false),
  _ideal(ideal),
  _subtract(subtract),
  _lowerBoundHint(0) {
  ASSERT(multiply.getVarCount() == ideal.getVarCount());
  ASSERT(multiply.getVarCount() == subtract.getVarCount());
}

const Term& HilbertSlice::getLcm() const {
#ifdef DEBUG
  if (_lcmUpdated) {
    Term tmp(_varCount);
	_ideal.getLcm(tmp);
    ASSERT(tmp == _lcm);
  }
#endif

  if (!_lcmUpdated) {
    getIdeal().getLcm(_lcm);
    _lcmUpdated = true;
  }
  return _lcm;
}

void HilbertSlice::print(FILE* file) const {
  fputs("HilbertSlice (multiply: ", file);
  _multiply.print(file);
  fputs("\n ideal: ", file);
  getIdeal().print(file);
  fputs(" subtract: ", file);
  _subtract.print(file);
}

void HilbertSlice::resetAndSetVarCount(size_t varCount) {
  _varCount = varCount;
  _ideal.clearAndSetVarCount(varCount);
  _subtract.clearAndSetVarCount(varCount);
  _multiply.reset(varCount);
  _lcm.reset(varCount);
  _lcmUpdated = true;
}

void HilbertSlice::clear() {
  _ideal.clear();
  _subtract.clear();
  _lcmUpdated = false;
}

void HilbertSlice::singleDegreeSortIdeal(size_t var) {
  _ideal.singleDegreeSort(var);
}

void HilbertSlice::insertIntoIdeal(const Exponent* term) {
  _ideal.insert(term);
  if (_lcmUpdated)
    _lcm.lcm(_lcm, term);
}

void HilbertSlice::swap(HilbertSlice& slice) {
  std::swap(_varCount, slice._varCount);
  _multiply.swap(slice._multiply);
  _lcm.swap(slice._lcm);
  std::swap(_lcmUpdated, slice._lcmUpdated);
  _ideal.swap(slice._ideal);
  _subtract.swap(slice._subtract);
}

bool HilbertSlice::innerSlice(const Term& pivot) {
  ASSERT(getVarCount() == pivot.getVarCount());

  size_t size = _ideal.getGeneratorCount();

  _multiply.product(_multiply, pivot);
  bool idealChanged = _ideal.colonReminimize(pivot);
  bool subtractChanged = _subtract.colonReminimize(pivot);
  bool changed = idealChanged || subtractChanged;
  if (changed) {
	normalize();
	_lowerBoundHint = pivot.getFirstNonZeroExponent();
  }

  if (_ideal.getGeneratorCount() == size)
    _lcm.colon(_lcm, pivot);
  else
    _lcmUpdated = false;

  return changed;
}

void HilbertSlice::outerSlice(const Term& pivot) {
  ASSERT(getVarCount() == pivot.getVarCount());

  size_t count = getIdeal().getGeneratorCount();
  _ideal.removeStrictMultiples(pivot);
  if (getIdeal().getGeneratorCount() != count)
    _lcmUpdated = false;

  if (pivot.getSizeOfSupport() > 1)
    getSubtract().insert(pivot);

  _lowerBoundHint = pivot.getFirstNonZeroExponent();
}

// TODO: rename, more elsewhere and integrate
void getCoef(Ideal& ideal, mpz_class& sum, bool negate) {
  size_t varCount = ideal.getVarCount();

  // This object is reused for several different purposes in order to
  // avoid havin to allocate and deallocate the underlying data
  // structure.
  Term term(varCount);

  // term is used as lcm to ensure all variables appear in ideal.
  ideal.getLcm(term);
  if (term.getSizeOfSupport() != varCount)
	return;

  // term is used to contain support counts to choose best pivot and
  // to detect base case.
  ideal.getSupportCounts(term);

  if (term.isSquareFree()) {
	if ((ideal.getGeneratorCount() % 2) == 1)
	  negate = !negate;
	if (negate)
	  sum -= 1;
	else
	  sum += 1;
	return;
  }
  size_t bestPivotVar = term.getFirstMaxExponent();

  // term is used to store pivot.
  term.setToIdentity();
  term[bestPivotVar] = 1;

  // Handle inner slice. The scope preserves memory resources by
  // deallocating the copied ideal early.
  Ideal inner(ideal);
  inner.colonReminimize(term);
  inner.insert(term); // to avoid having to keep more track of supports

  // Handle outer slice in-place using ideal.
  ideal.removeMultiples(term);
  ideal.insert(term);

  // inner is subtracted instead of added due to having added the
  // pivot to the ideal.
  getCoef(ideal, sum, negate);
  getCoef(inner, sum, !negate);
}

bool HilbertSlice::baseCase(CoefTermConsumer* consumer) {
  // Check that each variable appears in some minimal generator.
  if (getLcm().getSizeOfSupport() < _varCount)
    return true;

  if (!getLcm().isSquareFree())
	return false;

  mpz_class coef;
  getCoef(_ideal, coef, false);

  consumer->consume(coef, getMultiply());
  clear();
  return true;
}

void HilbertSlice::simplify() {
  ASSERT(!normalize());

  applyLowerBound();
  pruneSubtract();

  ASSERT(!normalize());
  ASSERT(!pruneSubtract());
  ASSERT(!applyLowerBound());
}

bool HilbertSlice::simplifyStep() {
  if (applyLowerBound())
	return true;

  pruneSubtract();
  return false;
}

// Helper class for normalize().
class StrictMultiplePredicate {
public:
  StrictMultiplePredicate(const Exponent* term, size_t varCount):
    _term(term), _varCount(varCount) {
  }

  bool operator()(const Exponent* term) {
    return ::strictlyDivides(_term, term, _varCount);
  }
  
private:
  const Exponent* _term;
  size_t _varCount;
};

bool HilbertSlice::normalize() {
  bool removedAny = false;

  Ideal::const_iterator stop = _subtract.end();
  for (Ideal::const_iterator it = _subtract.begin(); it != stop; ++it) {
    StrictMultiplePredicate pred(*it, _varCount);
    if (_ideal.removeIf(pred)) {
      removedAny = true;
      _lcmUpdated = false;
    }
  }

  return removedAny;
}

// Helper class for pruneSubtract().
class PruneSubtractPredicate {
public:
  PruneSubtractPredicate(const Ideal& ideal, const Term& lcm):
    _ideal(ideal), _lcm(lcm) {}

  bool operator()(const Exponent* term) {
    return
      !::strictlyDivides(term, _lcm, _lcm.getVarCount()) ||
      _ideal.contains(term);
  }
  
private:
  const Ideal& _ideal;
  const Term& _lcm;
};

bool HilbertSlice::pruneSubtract() {
  if (_subtract.getGeneratorCount() == 0)
    return false;

  PruneSubtractPredicate pred(getIdeal(), getLcm());
  return _subtract.removeIf(pred);
}

void HilbertSlice::applyTrivialLowerBound() {
  Term bound(_varCount);
  _ideal.getLeastExponents(bound);
  bound.decrement();
  if (!bound.isIdentity())
	innerSlice(bound);
}

bool HilbertSlice::applyLowerBound() {
  if (_ideal.getGeneratorCount() == 0)
    return false;

  bool changed = false;
  size_t stepsWithNoChange = 0;

  Term bound(_varCount);
  size_t var = _lowerBoundHint;
  while (stepsWithNoChange < _varCount) {
    if (!getLowerBound(bound, var)) {
      clear();
      return true;
    }

	if (!bound.isIdentity() && innerSlice(bound)) {
	  changed = true;
	  stepsWithNoChange = 0;
	} else
      ++stepsWithNoChange;

    var = (var + 1) % _varCount;
  }

  return changed;
}

bool HilbertSlice::getLowerBound(Term& bound, size_t var) const {
  bool seenAny = false;

  Ideal::const_iterator stop = getIdeal().end();
  for (Ideal::const_iterator it = getIdeal().begin(); it != stop; ++it) {
    if ((*it)[var] == 0)
      continue;

    if (seenAny)
      bound.gcd(bound, *it);
    else {
      bound = *it;
      seenAny = true;
    }
  }

  if (seenAny) {
    ASSERT(bound[var] >= 1);
    bound.decrement();
    return true;
  } else {
    // In this case the content is empty.
    return false;
  }
}

bool HilbertSlice::getLowerBound(Term& bound) const {
  ASSERT(_varCount > 0);
  Term tmp(_varCount);

  if (!getLowerBound(bound, 0))
    return false;
  for (size_t var = 1; var < _varCount; ++var) {
    if (!getLowerBound(tmp, var))
      return false;
    bound.lcm(bound, tmp);
  }

  return true;
}
