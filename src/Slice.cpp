#include "stdinc.h"
#include "Slice.h"

#include "Ideal.h"
#include "DecomConsumer.h"

Slice::Slice():
  _varCount(0),
  _multiply(),
  _lcm(),
  _ideal(),
  _subtract() {
}

Slice::Slice(const Ideal& ideal, const Ideal& subtract):
  _varCount(ideal.getVarCount()),
  _multiply(ideal.getVarCount()),
  _lcm(ideal.getVarCount()),
  _ideal(ideal),
  _subtract(subtract) {
  ASSERT(subtract.getVarCount() == ideal.getVarCount());
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

Slice::Slice(const Ideal& ideal,
	     const Ideal& subtract,
	     const Term& multiply, const Term& pivot):
  _varCount(multiply.getVarCount()),
  _multiply(multiply.getVarCount()),
  _lcm(multiply.getVarCount()),
  _ideal(ideal),
  _subtract(subtract) {
  ASSERT(multiply.getVarCount() == ideal.getVarCount());
  ASSERT(multiply.getVarCount() == subtract.getVarCount());
  ASSERT(multiply.getVarCount() == pivot.getVarCount());

  _ideal.colonReminimize(pivot);
  _subtract.colonReminimize(pivot);
  _multiply.product(multiply, pivot);
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

  getLowerBound(bound);
  if (bound.isIdentity())
    return false;

  _subtract.colonReminimize(bound);
  _ideal.colonReminimize(bound);
  _multiply.product(_multiply, bound);

  return true;
}

void Slice::getLowerBound(Term& bound, size_t var) const {
  bool seenAny = false;

  Ideal::const_iterator stop = _ideal.end();
  for (Ideal::const_iterator it = _ideal.begin(); it != stop; ++it) {
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
    bound[var] -= 1;
  } else
    bound.setToIdentity();
}

void Slice::getLowerBound(Term& bound) const {
  ASSERT(_varCount > 0);
  Term tmp(_varCount);

  getLowerBound(bound, 0);
  for (size_t var = 1; var < _varCount; ++var) {
    getLowerBound(tmp, var);
    bound.lcm(bound, tmp);
  }
}

bool Slice::twoVarBaseCase(DecomConsumer* consumer) {
  if (_varCount != 2)
    return false;

  _ideal.singleDegreeSort(0);

  static Term term(2);

  Ideal::const_iterator stop = _ideal.end();
  Ideal::const_iterator it = _ideal.begin();
  ASSERT(it != stop);

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
