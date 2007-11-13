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
  _ideal(ideal),
  _subtract(subtract) {
  ASSERT(multiply.getVarCount() == ideal.getVarCount());
  ASSERT(multiply.getVarCount() == subtract.getVarCount());
}

const Term& Slice::getLcm() const {
  _ideal.getLcm(_lcm);
  return _lcm;
}

void Slice::print(ostream& out) const {
  out << "Slice (multiply: " << _multiply << '\n'
      << " ideal: " << _ideal << '\n'
      << " subtract: " << _subtract << '\n'
      << ')';
}

void Slice::clearAndSetVarCount(size_t varCount) {
  _varCount = varCount;
  _ideal.clearAndSetVarCount(varCount);
  _subtract.clearAndSetVarCount(varCount);
  _multiply.reset(varCount);
  _lcm.reset(varCount);
}

void Slice::swap(Slice& slice) {
  std::swap(_varCount, slice._varCount);
  _multiply.swap(slice._multiply);
  _lcm.swap(slice._lcm);
  _ideal.swap(slice._ideal);
  _subtract.swap(slice._subtract);
}

void Slice::innerSlice(const Term& pivot) {
  ASSERT(getVarCount() == pivot.getVarCount());

  _ideal.colonReminimize(pivot);
  _subtract.colonReminimize(pivot);
  _multiply.product(_multiply, pivot);
  normalize();
}

bool Slice::baseCase(DecomConsumer* consumer) {
  if (twoVarBaseCase(consumer))
    return true;

  const Term& lcm = getLcm();

  // Check that each variable appears in some minimal generator.
  if (lcm.getSizeOfSupport() != _varCount)
    return true;

  // Check that ideal is square free.
  if (!lcm.isSquareFree())
    return false;

  // We have reached the square free base case. The content is empty
  // unless ideal has the form <x_1, ..., x_n>.
  if (_ideal.getGeneratorCount() == _varCount &&
      _ideal.isIrreducible())
    consumer->consume(_multiply);

  return true;
}

void Slice::simplify() {
  // It would make sense to normalize here, but we ensure elsewhere
  // that the slice is already normalized. This ASSERT is here to
  // catch it if this changes.
  ASSERT(!normalize());
  removeDoubleLcm();

  while (applyLowerBound()) {
    while (applyLowerBound())
      ;

    bool changed1 = normalize();
    bool changed2 = removeDoubleLcm();

    if (!changed1 && !changed2)
      break;
  }

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
    if (_ideal.removeIf(pred))
      removedAny = true;
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

  PruneSubtractPredicate pred(_ideal, getLcm());
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
  bool removedAny = false;

  while (true) {
    DoubleLcmPredicate pred(getLcm());
    if (!_ideal.removeIf(pred))
      break;

    removedAny = true;
  };

  return removedAny;
}

bool Slice::applyLowerBound() {
  Term bound(_varCount);

  if (!getLowerBound(bound)) {
    _ideal.clear();
    _subtract.clear();
    return false;
  }

  if (bound.isIdentity())
    return false;

  _subtract.colonReminimize(bound);
  _ideal.colonReminimize(bound);
  _multiply.product(_multiply, bound);

  return true;
}

bool Slice::getLowerBound(Term& bound, size_t var) const {
  bool seenAny = false;

  //const Term& lcm = getLcm();
  const Term& lcm = _lcm; // TODO: fix

  Ideal::const_iterator stop = _ideal.end();
  for (Ideal::const_iterator it = _ideal.begin(); it != stop; ++it) {
    if ((*it)[var] == 0)
      continue;
    
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

  _ideal.singleDegreeSort(0);

  static Term term(2);

  Ideal::const_iterator stop = _ideal.end();
  Ideal::const_iterator it = _ideal.begin();
  if (it == stop)
    return true;

  while (true) {
    term[1] = (*it)[1] - 1;
    
    ++it;
    if (it == stop)
      break;

    term[0] = (*it)[0] - 1;

    ASSERT(!_ideal.contains(term));

    if (!_subtract.contains(term)) {
      term[0] += _multiply[0];
      term[1] += _multiply[1];
      
      consumer->consume(term);
    }
  }

  return true;
}
