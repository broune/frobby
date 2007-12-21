#include "stdinc.h"
#include "Slice.h"

#include "Ideal.h"
#include "DecomConsumer.h"

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

void Slice::print(ostream& out) const {
  out << "Slice (multiply: " << _multiply << '\n'
      << " ideal: " << getIdeal() << '\n'
      << " subtract: " << _subtract << '\n'
      << ')';
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

bool Slice::baseCase(DecomConsumer* consumer) {
  if (_varCount == 1) {
    consumer->consume(_multiply);
    return true;
  }

  if (twoVarBaseCase(consumer))
    return true;

  // Check that each variable appears in some minimal generator.
  if (getLcm().getSizeOfSupport() < _varCount)
    return true;

  if (getIdeal().getGeneratorCount() > _varCount)
    return false;
  
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
    if (!bound.isIdentity()) {
      if (bound.getSizeOfSupport() == 1 &&
	  bound.getFirstNonZeroExponent() == var) {
	bool trivial = true;
	Ideal::const_iterator stop = _ideal.end();
	for (Ideal::const_iterator it = getIdeal().begin(); it != stop; ++it) {
	  if (0 < (*it)[var] && (*it)[var] <= bound[var]) {
	    trivial = false;
	    break;
	  }
	}
	stop = _subtract.end();
	for (Ideal::const_iterator it = _subtract.begin(); it != stop; ++it) {
	  if (0 < (*it)[var] && (*it)[var] <= bound[var]) {
	    trivial = false;
	    break;
	  }
	}


	if (trivial) {
	  innerSlice(bound);
	  changed = true;
	  ++stepsWithNoChange;
	  goto bob;
	}
      }

      innerSlice(bound);
      stepsWithNoChange = 0;
      changed = true;
    } else
      ++stepsWithNoChange;

  bob:
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

bool Slice::twoVarBaseCase(DecomConsumer* consumer) {
  if (_varCount != 2)
    return false;

  singleDegreeSortIdeal(0);

  static Term term(2);

  Ideal::const_iterator stop = getIdeal().end();
  Ideal::const_iterator it = getIdeal().begin();
  if (it == stop)
    return true;

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

  return true;
}

void Slice::validate() const {
#ifdef DEBUG
  getLcm();
#endif
}
