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
#include "Slice.h"

#include "Projection.h"

// The lcm is technically correct, but _lcmUpdated defaulting to false
// is still a sensible choice.
Slice::Slice():
  _varCount(0),
  _lcmUpdated(false),
  _lowerBoundHint(0) {
}

Slice::Slice(const Ideal& ideal, const Ideal& subtract, const Term& multiply):
  _ideal(ideal),
  _subtract(subtract),
  _multiply(multiply),
  _varCount(multiply.getVarCount()),
  _lcm(multiply.getVarCount()),
  _lcmUpdated(false),
  _lowerBoundHint(0) {
  ASSERT(multiply.getVarCount() == ideal.getVarCount());
  ASSERT(multiply.getVarCount() == subtract.getVarCount());
}

Slice::~Slice() {
  // We are defining this do-nothing destructor in order to make it
  // virtual.
}

const Term& Slice::getLcm() const {
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

void Slice::print(FILE* file) const {
  fputs("Slice (multiply: ", file);
  _multiply.print(file);
  fputs("\n ideal: ", file);
  getIdeal().print(file);
  fputs(" subtract: ", file);
  _subtract.print(file);
}

Slice& Slice::operator=(const Slice& slice) {
  _varCount = slice._varCount;
  _ideal = slice._ideal;
  _subtract = slice._subtract;
  _multiply = slice._multiply;
  _lcm = slice._lcm;
  _lcmUpdated = slice._lcmUpdated;
  _lowerBoundHint = slice._lowerBoundHint;  

  return *this;
}

void Slice::resetAndSetVarCount(size_t varCount) {
  _varCount = varCount;
  _ideal.clearAndSetVarCount(varCount);
  _subtract.clearAndSetVarCount(varCount);
  _multiply.reset(varCount);
  _lcm.reset(varCount);
  _lcmUpdated = false;
  _lowerBoundHint = 0;
}

void Slice::clearIdealAndSubtract() {
  _ideal.clear();
  _subtract.clear();
  _lcmUpdated = false;
  _lowerBoundHint = 0;
}

void Slice::singleDegreeSortIdeal(size_t var) {
  _ideal.singleDegreeSort(var);
}

void Slice::insertIntoIdeal(const Exponent* term) {
  _ideal.insert(term);
  if (_lcmUpdated)
    _lcm.lcm(_lcm, term);
}

bool Slice::innerSlice(const Term& pivot) {
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

void Slice::outerSlice(const Term& pivot) {
  ASSERT(getVarCount() == pivot.getVarCount());

  size_t count = getIdeal().getGeneratorCount();
  _ideal.removeStrictMultiples(pivot);
  if (getIdeal().getGeneratorCount() != count)
    _lcmUpdated = false;

  if (pivot.getSizeOfSupport() > 1)
    getSubtract().insertReminimize(pivot);

  _lowerBoundHint = pivot.getFirstNonZeroExponent();
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

bool Slice::normalize() {
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

void Slice::setToProjOf
(const Slice& slice, const Projection& projection) {
  resetAndSetVarCount(projection.getRangeVarCount());

  Ideal::const_iterator stop = slice.getIdeal().end();
  for (Ideal::const_iterator it = slice.getIdeal().begin();
	   it != stop; ++it) {

    size_t var = getFirstNonZeroExponent(*it, slice.getVarCount());
	if (var == slice.getVarCount() || projection.domainVarHasProjection(var)) {
	  projection.project(_lcm, *it);
	  insertIntoIdeal(_lcm);
	}
  }

  stop = slice.getSubtract().end();
  for (Ideal::const_iterator it = slice.getSubtract().begin();
	   it != stop; ++it) {

    size_t var = getFirstNonZeroExponent(*it, slice.getVarCount());
	if (var == slice.getVarCount() || projection.domainVarHasProjection(var)) {
	  projection.project(_lcm, *it);
	  getSubtract().insert(_lcm);
	}
  }

  projection.project(getMultiply(), slice.getMultiply());
  if (slice._lcmUpdated) {
	projection.project(_lcm, slice._lcm);
	_lcmUpdated = true;
  } else
	_lcmUpdated = false;
}

void Slice::swap(Slice& slice) {
  std::swap(_varCount, slice._varCount);
  _multiply.swap(slice._multiply);
  _lcm.swap(slice._lcm);
  std::swap(_lcmUpdated, slice._lcmUpdated);
  _ideal.swap(slice._ideal);
  _subtract.swap(slice._subtract);
  std::swap(_lowerBoundHint, slice._lowerBoundHint);
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
  void operator=(const PruneSubtractPredicate&); // To make inaccessible.

  const Ideal& _ideal;
  const Term& _lcm;
};

bool Slice::pruneSubtract() {
  if (_subtract.getGeneratorCount() == 0)
    return false;

  PruneSubtractPredicate pred(getIdeal(), getLcm());
  return _subtract.removeIf(pred);
}

bool Slice::applyLowerBound() {
  if (_ideal.getGeneratorCount() == 0)
    return false;

  bool changed = false;
  size_t stepsWithNoChange = 0;

  Term bound(_varCount);
  size_t var = _lowerBoundHint;
  while (stepsWithNoChange < _varCount) {
    if (!getLowerBound(bound, var)) {
      clearIdealAndSubtract();
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
