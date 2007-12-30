#include "stdinc.h"
#include "Slice.h"

#include "Ideal.h"
#include "TermConsumer.h"

Slice::Slice():
  _varCount(0) {
}

Slice::Slice(const Ideal& ideal, const Ideal& subtract,
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
  fputs("\n subtract: ", file);
  _subtract.print(file);
  fputc('\n', file);
}

void Slice::resetAndSetVarCount(size_t varCount) {
  _varCount = varCount;
  _ideal.clearAndSetVarCount(varCount);
  _subtract.clearAndSetVarCount(varCount);
  _multiply.reset(varCount);
  _lcm.reset(varCount);
  _lcmUpdated = true;
}

void Slice::clear() {
  _ideal.clear();
  _subtract.clear();
  _lcmUpdated = false;
}

void Slice::singleDegreeSortIdeal(size_t var) {
  _ideal.singleDegreeSort(var);
}

void Slice::insertIntoIdeal(const Exponent* term) {
  _ideal.insert(term);
  if (_lcmUpdated)
    _lcm.lcm(_lcm, term);
}

void Slice::swap(Slice& slice) {
  std::swap(_varCount, slice._varCount);
  _multiply.swap(slice._multiply);
  _lcm.swap(slice._lcm);
  std::swap(_lcmUpdated, slice._lcmUpdated);
  _ideal.swap(slice._ideal);
  _subtract.swap(slice._subtract);
}

void Slice::innerSlice(const Term& pivot) {
  ASSERT(getVarCount() == pivot.getVarCount());

  size_t size = _ideal.getGeneratorCount();

  _ideal.colonReminimize(pivot);
  _subtract.colonReminimize(pivot);
  _multiply.product(_multiply, pivot);
  normalize();

  if (_ideal.getGeneratorCount() == size)
    _lcm.colon(_lcm, pivot);
  else
    _lcmUpdated = false;

  _lowerBoundHint = pivot.getFirstNonZeroExponent();
}

void Slice::outerSlice(const Term& pivot) {
  ASSERT(getVarCount() == pivot.getVarCount());

  size_t count = getIdeal().getGeneratorCount();
  _ideal.removeStrictMultiples(pivot);
  if (getIdeal().getGeneratorCount() != count)
    _lcmUpdated = false;

  if (pivot.getSizeOfSupport() > 1)
    getSubtract().insert(pivot);

  _lowerBoundHint = pivot.getFirstNonZeroExponent();
}

bool Slice::baseCase(TermConsumer* consumer) {
  if (getIdeal().getGeneratorCount() < _varCount)
    return true;

  // Check that each variable appears in some minimal generator.
  if (getLcm().getSizeOfSupport() < _varCount)
    return true;

  if (_varCount == 1) {
    consumer->consume(_multiply);
    return true;
  }
  if (_varCount == 2) {
    twoVarBaseCase(consumer);
    return true;
  }

  if (getIdeal().getGeneratorCount() > _varCount) {
    if (getIdeal().getGeneratorCount() == _varCount + 1) {
      oneMoreGeneratorBaseCase(consumer);
      return true;
    }
    if (twoNonMaxBaseCase(consumer))
      return true;
    
    return false;
  }

  // These things are ensured since the slice is simplified.
  ASSERT(getLcm().isSquareFree());
  ASSERT(getIdeal().isIrreducible());

  consumer->consume(_multiply);
  return true;
}

void Slice::simplify() {
  removeDoubleLcm();
  while (applyLowerBound() &&
	 removeDoubleLcm())
    ;

  pruneSubtract();

  ASSERT(!normalize());
  ASSERT(!pruneSubtract());
  ASSERT(!removeDoubleLcm());
  ASSERT(!applyLowerBound());
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

bool Slice::pruneSubtract() {
  if (_subtract.getGeneratorCount() == 0)
    return false;

  PruneSubtractPredicate pred(getIdeal(), getLcm());
  return _subtract.removeIf(pred);
}

// Helper class for removeDoubleLcm().
class DoubleLcmPredicate {
public:
  DoubleLcmPredicate(const Term& lcm):
    _lcm(lcm) {
  }

  bool operator()(const Exponent* term) {
    bool seenMatch = false;
    for (size_t var = 0; var < _lcm.getVarCount(); ++var) {
      if (term[var] == _lcm[var]) {
	if (seenMatch)
	  return true;
	seenMatch = true;
      }
    }
    return false;
  }

private:
  const Term& _lcm;
};

bool Slice::removeDoubleLcm() {
  if (_ideal.getGeneratorCount() == 0)
    return false;

  bool removedAny = false;

  while (true) {
    DoubleLcmPredicate pred(getLcm());
    if (!_ideal.removeIf(pred))
      break;

    removedAny = true;
    _lcmUpdated = false;
  };

  return removedAny;
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
      clear();
      return true;
    }

    size_t sizeOfSupport = bound.getSizeOfSupport();
    if (sizeOfSupport > 0) {
      if (sizeOfSupport == 1 &&
	  bound.getFirstNonZeroExponent() == var &&
	  isTrivialColon(var, bound[var])) {
	// In this case, there is no need for reminimization or
	// re-lower-bounding of this variable.
	_ideal.colon(bound);
	_subtract.colon(bound);
	_multiply.product(_multiply, bound);
	_lcm.colon(_lcm, bound);
	++stepsWithNoChange;
      } else {
	innerSlice(bound);
	stepsWithNoChange = 0;
	changed = true;
      }
    } else
      ++stepsWithNoChange;

    var = (var + 1) % _varCount;
  }

  return changed;
}

bool Slice::getLowerBound(Term& bound, size_t var) const {
  bool seenAny = false;

  const Term& lcm = getLcm();

  Ideal::const_iterator stop = getIdeal().end();
  for (Ideal::const_iterator it = getIdeal().begin(); it != stop; ++it) {
    if ((*it)[var] == 0)
      continue;
        
    // Use the fact that terms with a maximal exponent somewhere not
    // at var cannot be a var-label.
    bool relevant = true;
    for (size_t var2 = 0; var2 < _varCount; ++var2) {
      if (var2 != var && (*it)[var2] == lcm[var2]) {
	relevant = false;
	break;
      }
    }
    
    if (!relevant)
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
    bound[var] -= 1;
    return true;
  } else {
    // In this case the content is empty.
    return false;
  }
}

bool Slice::getLowerBound(Term& bound) const {
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

void Slice::twoVarBaseCase(TermConsumer* consumer) {
  ASSERT(_varCount == 2);

  singleDegreeSortIdeal(0);

  static Term term(2);

  Ideal::const_iterator stop = getIdeal().end();
  Ideal::const_iterator it = getIdeal().begin();
  if (it == stop)
    return;

  while (true) {
    term[1] = (*it)[1] - 1;
    
    ++it;
    if (it == stop)
      break;

    term[0] = (*it)[0] - 1;

    ASSERT(!getIdeal().contains(term));

    if (!_subtract.contains(term)) {
      term[0] += _multiply[0];
      term[1] += _multiply[1];
      
      consumer->consume(term);
    }
  }
}

void Slice::oneMoreGeneratorBaseCase(TermConsumer* consumer) {
  ASSERT(_varCount + 1 == getIdeal().getGeneratorCount());

  // Since the slice is fully simplified, we must be dealing with an
  // artinian power in each variable, and then a single generator
  // whose support is at least 2. We can then simply run through all
  // the possibilities for that generator to be a label.

  Ideal::const_iterator it = getIdeal().begin();
  while (getSizeOfSupport(*it, _varCount) == 1) {
    ++it;
    ASSERT(it != getIdeal().end());
  }

  for (size_t var = 0; var < _varCount; ++var) {
    ASSERT((*it)[var] <= 1);
    ASSERT((*it)[var] < getLcm()[var]);
    if ((*it)[var] == 1)
      _multiply[var] += getLcm()[var] - 1;
  }

  for (size_t var = 0; var < _varCount; ++var) {
    if ((*it)[var] == 1) {
      (*it)[var] = 0;
      if (!getSubtract().contains(*it)) {
	_multiply[var] -= getLcm()[var] - 1;
	consumer->consume(_multiply);
	_multiply[var] += getLcm()[var] - 1;
      }
      (*it)[var] = 1;
    }
  }
}

// Helper function for the method twoNonMaxBaseCase.
bool getTheOnlyTwoNonMax(Ideal::const_iterator it,
			 const Exponent*& first,
			 const Exponent*& second,
			 Ideal::const_iterator end,
			 const Term& lcm) {
  size_t count = 0;
  for (; it != end; ++it) {
    bool nonMax = true;
    for (size_t var = 0; var < lcm.getVarCount(); ++var) {
      if ((*it)[var] == lcm[var]) {
	nonMax = false;
	break;
      }
    }
    if (nonMax) {
      if (count == 0)
	first = *it;
      else if (count == 1)
	second = *it;
      else
	return false;
      ++count;
    }
  }
  return count == 2;
}

bool Slice::twoNonMaxBaseCase(TermConsumer* consumer) {
  const Term& lcm = getLcm();
  Ideal::const_iterator stop = getIdeal().end();

  const Exponent* nonMax1;
  const Exponent* nonMax2;
  if (!getTheOnlyTwoNonMax(getIdeal().begin(), nonMax1, nonMax2, stop, lcm))
    return false;

  Term msm(_lcm);
  for (size_t var = 0; var < _varCount; ++var)
    msm[var] -= 1;
  Term tmp(_varCount);

  for (size_t var1 = 0; var1 < _varCount; ++var1) {
    if (nonMax1[var1] == 0)
      continue;
    if (nonMax1[var1] <= nonMax2[var1])
      continue;

    for (size_t var2 = 0; var2 < _varCount; ++var2) {
      if (var1 == var2 || nonMax2[var2] == 0)
	continue;
      if (nonMax2[var2] <= nonMax1[var2])
	continue;
      
      // Use tmp to record those variables for which labels have been
      // found. If some variable has no label, then we are not dealing
      // with an actual maximal standard monomial.
      tmp.setToIdentity();
      tmp[var1] = true;
      tmp[var2] = true;
      for (Ideal::const_iterator it = getIdeal().begin(); it != stop; ++it) {
	if ((*it)[var1] >= nonMax1[var1] ||
	    (*it)[var2] >= nonMax2[var2])
	  continue;

	for (size_t var = 0; var < lcm.getVarCount(); ++var) {
	  if ((*it)[var] == lcm[var]) {
	    tmp[var] = true;
	    break;
	  }
	}
      }

      if (tmp.getSizeOfSupport() < _varCount)
	continue;
      
      msm[var1] = nonMax1[var1] - 1;
      msm[var2] = nonMax2[var2] - 1;
      if (!getSubtract().contains(msm)) {
	tmp.product(msm, _multiply);
	consumer->consume(tmp);
      }
      msm[var2] = lcm[var2] - 1;
      msm[var1] = lcm[var1] - 1;
    }
  }

  for (size_t var = 0; var < _varCount; ++var) {
    Exponent e;
    if (nonMax1[var] < nonMax2[var])
      e = nonMax1[var];
    else
      e = nonMax2[var];
    if (e == 0)
      continue;

    msm[var] = e - 1;
    if (!getSubtract().contains(msm)) {
      tmp.product(msm, _multiply);
      consumer->consume(tmp);
    }
    msm[var] = lcm[var] - 1;
  }

  return true;
}

bool Slice::isTrivialColon(size_t var, size_t exponent) {
  Ideal::const_iterator stop = _ideal.end();
  for (Ideal::const_iterator it = getIdeal().begin(); it != stop; ++it)
    if (0 < (*it)[var] && (*it)[var] <= exponent)
      return false;

  stop = _subtract.end();
  for (Ideal::const_iterator it = _subtract.begin(); it != stop; ++it)
    if (0 < (*it)[var] && (*it)[var] <= exponent)
      return false;

  return true;
}
