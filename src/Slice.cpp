#include "stdinc.h"
#include "Slice.h"

#include "Ideal.h"
#include "DecomConsumer.h"

class PruneSubtractFilter : public Ideal::FilterFunction {
public:
  PruneSubtractFilter(const Term& lcm):
    _lcm(lcm) {}

  virtual bool operator()(const Exponent* term) {
    return ::strictlyDivides(term, _lcm, _lcm.getVarCount());
  }

private:
  const Term& _lcm;
};

class DoubleLcmFilter : public Ideal::FilterFunction {
public:
  DoubleLcmFilter(size_t varCount):
    _lcm(0),
    _varCount(varCount) {
  }

  void setLcm(const Exponent* lcm) {
    _lcm = lcm;
  }

  virtual bool operator()(const Exponent* term) {
    bool seenMatch = false;
    for (size_t var = 0; var < _varCount; ++var) {
      if (term[var] == _lcm[var]) {
	if (seenMatch)
	  return false;
	seenMatch = true;
      }
    }
    return true;
  }

private:
  const Exponent* _lcm;
  size_t _varCount;
};

bool Slice::twoVarBaseCase(DecomConsumer* consumer) {
  if (_varCount != 2)
    return false;

  _ideal->singleDegreeSort(0);

  static Term term(2);

  Ideal::const_iterator stop = _ideal->end();
  Ideal::const_iterator it = _ideal->begin();
  ASSERT(it != stop);

  while (true) {
    term[1] = _multiply[1] + (*it)[1] - 1;
    
    ++it;
    if (it == stop)
      break;

    term[0] = _multiply[0] + (*it)[0] - 1;

    // TODO: take subtract into account
    consumer->consume(term);
  }

  return true;
}

bool Slice::baseCase(DecomConsumer* consumer) {
  // It is assumed that the slice is normalized.

  if (twoVarBaseCase(consumer))
    return true;

  const Term& lcm = getLcm();

  // Check that each variable appears in some minimal generator.
  if (lcm.getSizeOfSupport() != _varCount)
    return true;

  // Check that ideal is square free.
  if (!lcm.isSquareFree())
    return false;

  // We have reached the square free base case. The content is
  // non-empty if and only if ideal has the form <x_1, ..., x_n>.
  if (_ideal->getGeneratorCount() != _varCount)
    return true;

  // This might appear to rest on an assumption that ideal is
  // minimized and thus has no duplicates. This is not the case, since
  // we checked that each variable appears in some minimal generator.
  // TODO: add a method isIrreducible to Ideal.
  Ideal::const_iterator stop = _ideal->end();
  for (Ideal::const_iterator it = _ideal->begin(); it != stop; ++it)
    if (getSizeOfSupport(*it, _varCount) != 1)
      return true;

  consumer->consume(_multiply);
  return true;
}

void Slice::simplify() {
  ASSERT(!normalize());

  Term tmp(_varCount);
  Term lcm(_varCount);
  Term gcd(_varCount);

  filterDoubleLcm();

 beginning:
  bool changed = false;

  while (true) {
    Ideal::const_iterator idealBegin = _ideal->begin();
    Ideal::const_iterator idealEnd = _ideal->end();

    lcm.setToIdentity();
    for (size_t offset = 0; offset < _varCount; ++offset) {
      bool first = true;
      for (Ideal::const_iterator it = idealBegin; it != idealEnd; ++it) {
	if ((*it)[offset] == 0)
	  continue;
	if (first) {
	  tmp = *it;
	  first = false;
	} else
	  tmp.gcd(tmp, *it);
      }
      if (first)
	return; // we reached a basecase, so no reason to simplify further
      if (!first) {
	tmp[offset] -= 1;
	lcm.lcm(lcm, tmp);
      }
    }    

    if (lcm.isIdentity())
      break;

    changed = true;

    _subtract->colon(lcm);
    _ideal->colonReminimize(lcm);
    _multiply.product(_multiply, lcm);
  }

  if (changed) {
    bool b1 = normalize();
    bool b2 = filterDoubleLcm();

    if (b1 || b2)
      goto beginning;
  }

  _subtract->minimize();
}

bool Slice::normalize() {
  if (_subtract->getGeneratorCount() == 0)
    return false;

  bool removedAny = false;
  for (Ideal::const_iterator it = _subtract->begin();
       it != _subtract->end(); ++it)
    if (_ideal->removeStrictMultiples(*it))
      removedAny = true;

  PruneSubtractFilter pruneFilter(getLcm());
  _subtract->filter(pruneFilter);

  return removedAny;
}

bool Slice::filterDoubleLcm() {
  bool removedAny = false;

  DoubleLcmFilter filter(_varCount);
  while (true) {
    filter.setLcm(getLcm());
    if (!_ideal->filter(filter))
      break;

    removedAny = true;
  };
  
  return removedAny;
}

Slice::Slice(Ideal* ideal,
	     Ideal* subtract,
	     size_t varCount):
  _varCount(varCount),
  _multiply(varCount),
  _lcm(varCount),
  _ideal(ideal),
  _subtract(subtract) {
  ASSERT(varCount == ideal->getVariableCount());
  ASSERT(varCount == subtract->getVariableCount());
}

Slice::Slice(Ideal* ideal,
	     Ideal* subtract,
	     const Term& multiply):
  _varCount(multiply.getVarCount()),
  _multiply(multiply),
  _lcm(multiply.getVarCount()),
  _ideal(ideal),
  _subtract(subtract) {
  ASSERT(multiply.getVarCount() == ideal->getVariableCount());
  ASSERT(multiply.getVarCount() == subtract->getVariableCount());
}

Slice::Slice(Ideal* ideal,
	     Ideal* subtract,
	     const Term& multiply, const Term& pivot):
  _varCount(multiply.getVarCount()),
  _multiply(multiply.getVarCount()),
  _lcm(multiply.getVarCount()),
  _ideal(ideal),
  _subtract(subtract) {
  ASSERT(multiply.getVarCount() == ideal->getVariableCount());
  ASSERT(multiply.getVarCount() == subtract->getVariableCount());

  _multiply.product(multiply, pivot);
}

Slice::~Slice() {
  delete _ideal;
  delete _subtract;
}

const Term& Slice::getLcm() const {
  _ideal->getLcm(_lcm);
  return _lcm;
}

void Slice::print() const {
  cerr << "SLICE: Multiply: " << _multiply << endl;
  cerr << "Ideal: ";
  _ideal->print();

  cerr << "Subtract: ";
  _subtract->print();

  cerr << flush;
}
