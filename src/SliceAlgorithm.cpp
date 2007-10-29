#include "stdinc.h"
#include "SliceAlgorithm.h"

#include "TermList.h"
#include "Term.h"
#include "VarNames.h"
#include "io.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "Partition.h"
#include "TermList.h"

// TODO: move
unsigned int level = 0;
vector<unsigned int> calls;

class DecomWriter : public SliceAlgorithm::DecomConsumer {
public:
  DecomWriter(const VarNames& names,
	      const TermTranslator* translator, 
	      ostream& out):
    _varCount(names.getVarCount()),
    _names(names),
    _translator(translator),
    _out(out) {
    // TODO: improve output configurability.
    _ioHandler = IOHandler::createIOHandler("monos");
    _ioHandler->startWritingIdeal(out, names);
  }

  virtual ~DecomWriter() {
    _ioHandler->doneWritingIdeal(_out);
    _out << flush;

    delete _ioHandler;
  }
  
  virtual void consume(const Term& term) {
    vector<const char*> tmp(_varCount);
    for (size_t var = 0; var < _varCount; ++var)
      tmp[var] = (_translator->getExponentString(var, term[var] + 1));
    _ioHandler->writeGeneratorOfIdeal(_out, tmp, _names);
  }
  
private:
  size_t _varCount;

  IOHandler* _ioHandler;
  VarNames _names;
  const TermTranslator* _translator;
  ostream& _out;
};

class DecomStorer : public SliceAlgorithm::DecomConsumer {
public:
  // The DecomStorer does NOT take over ownership of decom.
  DecomStorer(Ideal* decom):
    _decom(decom) {
  }
  virtual ~DecomStorer() {
  }

  virtual void consume(const Term& term) {
    _decom->insert(term);
  }

private:
  Ideal* _decom;
};

SliceAlgorithm::SliceAlgorithm(const Ideal& ideal,
			       const VarNames& names,
			       const TermTranslator* translator,
			       ostream& out) {
  _decomConsumer = new DecomWriter(names, translator, out);

  if (!ideal.isZeroIdeal()) {
    Slice slice(ideal.clone(),
		new TermList(ideal.getVariableCount()),
		Term(ideal.getVariableCount()));
    content(slice);
  }

  delete _decomConsumer;

  // TODO: move
  unsigned int sum = 0;
  for (unsigned int level = 0; level < calls.size(); ++level) {
    sum += calls[level];
    cerr << "Level " << level + 1 << " had "
	 << calls[level] << " calls. " << endl;
  }
  cerr << endl << "Sum: " << sum << endl;
}

SliceAlgorithm::ProjDecom::~ProjDecom() {
	delete decom;
}

bool SliceAlgorithm::independenceSplit(Slice& slice) {
  // Compute the partition
  Partition partition(slice.getVarCount());

  Ideal::const_iterator idealEnd = slice.getIdeal()->end();
  for (Ideal::const_iterator it = slice.getIdeal()->begin();
       it != idealEnd; ++it) {
    size_t first = ::getFirstNonZeroExponent(*it, slice.getVarCount());
    for (size_t var = first + 1; var < slice.getVarCount(); ++var)
      if ((*it)[var] > 0)
	partition.join(first, var);
  }

  // Check if there are any independent subsets of variables.
  size_t setCount = partition.getSetCount();
  if (setCount <= 1)
    return false;

  vector<ProjDecom> projDecoms(setCount);

  for (size_t i = 0; i < setCount; ++i) {
    vector<Exponent> decompressor;
    vector<Exponent>& compressor = projDecoms[i].compressor;

    partition.getSetTranslators(i, compressor, decompressor);
    size_t projVarCount = partition.getSetSize(i);

    Slice projSlice(slice.getIdeal()->createNew(projVarCount),
	       slice.getSubtract()->createNew(projVarCount),
	       projVarCount);

    partition.project(projSlice.getMultiply(), slice.getMultiply(),
		      compressor);
    Term tmp(projVarCount);
    for (Ideal::const_iterator it = slice.getIdeal()->begin();
	 it != idealEnd; ++it) {
      partition.project(tmp, *it, compressor);
      if (!tmp.isIdentity())
	projSlice.getIdeal()->insert(tmp);
    }

    DecomConsumer* oldConsumer = _decomConsumer;
    projDecoms[i].decom = slice.getIdeal()->createNew(projVarCount);
    _decomConsumer = new DecomStorer(projDecoms[i].decom);
    
    content(projSlice);
    
    delete _decomConsumer;
    _decomConsumer = oldConsumer;
  }

  Term term(slice.getVarCount());
  inverseDecomProject(projDecoms, partition, 0, term);

  return true;
}

// TODO, BUG: should use subtract
void SliceAlgorithm::inverseDecomProject(const vector<ProjDecom>& projs,
					 Partition& partition,
					 size_t proj,
					 Term& term) {
  if (proj == projs.size()) {
    _decomConsumer->consume(term);
    return;
  }

  
  Ideal::const_iterator stop = projs[proj].decom->end();
  for (Ideal::const_iterator it = projs[proj].decom->begin();
       it != stop; ++it) {
    partition.inverseProject(term, *it, projs[proj].compressor);
    inverseDecomProject(projs, partition, proj + 1, term);
  }
}

void SliceAlgorithm::labelSplit(Slice& slice) {
  size_t var = slice.getLcm().getFirstMaxExponent();
  slice.getIdeal()->singleDegreeSort(var);

  Ideal* cumulativeSubtract = slice.getSubtract();
  Term labelMultiply(slice.getVarCount());
  Term pivot(slice.getVarCount());

  for (Ideal::const_iterator it = slice.getIdeal()->begin();
       it != slice.getIdeal()->end(); ++it) {
    if ((*it)[var] == 0)
      continue;

    pivot = *it;
    pivot[var] -= 1;

    {
      Slice child(slice.getIdeal()->createMinimizedColon(pivot),
		  cumulativeSubtract->createMinimizedColon(pivot),
		  slice.getMultiply(), pivot);
      child.normalize();
      content(child);
    }

    Ideal::const_iterator next = it;
    ++next;
    if (next != slice.getIdeal()->end() && (*it)[var] == (*next)[var]) {
      if (cumulativeSubtract == slice.getSubtract())
	cumulativeSubtract = slice.getSubtract()->clone();
      cumulativeSubtract->insert(pivot);
    } else if (cumulativeSubtract != slice.getSubtract()) {
      delete cumulativeSubtract;
      cumulativeSubtract = slice.getSubtract();
    }
  }
}

void SliceAlgorithm::pivotSplit(Slice& slice) {
  Term pivot(slice.getVarCount());
  getPivot(slice, pivot);

  {
    Slice inner(slice.getIdeal()->createMinimizedColon(pivot),
		slice.getSubtract()->createMinimizedColon(pivot),
		slice.getMultiply(), pivot);
    content(inner);
  }

  {
    Slice outer(slice.getIdeal()->clone(),
		slice.getSubtract()->clone(),
		slice.getMultiply());
    outer.addToSubtract(pivot);
    
    content(outer);
  }
}

void SliceAlgorithm::content(Slice& slice) {
  slice.simplify();

  if (calls.size() == level)
    calls.push_back(0);
  ++calls[level];
  ++level;

  if (!slice.baseCase(_decomConsumer)) {
    if (!independenceSplit(slice)) {
      if (false)
	labelSplit(slice);
      else
	pivotSplit(slice);
    }
  }

  --level;
}

// TODO: make the pivot selection strategy a parameter.
void SliceAlgorithm::getPivot(const Slice& slice,
			      Term& pivot) {
  size_t maxOffset = slice.getLcm().getFirstMaxExponent();
  pivot.setToIdentity();
  pivot[maxOffset] = 1;

  ASSERT(!pivot.isIdentity()); 
  ASSERT(!slice.getIdeal()->contains(pivot));
  ASSERT(!slice.getSubtract()->contains(pivot));
}

class IrreducibleNormalizeFilter : public Ideal::FilterFunction {
public:
  IrreducibleNormalizeFilter(size_t varCount):
    _irrIdeal(varCount) {}

  Term& irrIdeal() {
    return _irrIdeal;
  }

  virtual bool operator()(const Exponent* term) {
    return _irrIdeal.dominates(term);
  }

private:
  Term _irrIdeal;
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

bool SliceAlgorithm::Slice::baseCase
(SliceAlgorithm::DecomConsumer* consumer) const {
  // It is assumed that the slice is normalized.

  const Term& lcm = getLcm(); // recalculate

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

void SliceAlgorithm::Slice::simplify() {
  Term tmp(_varCount);
  Term lcm(_varCount);
  Term gcd(_varCount);

  normalize();
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

bool SliceAlgorithm::Slice::normalize() {
  if (_varCount == 0)
    return false;

  bool irreducible = true;

  IrreducibleNormalizeFilter filter(_varCount);
  Term& t = filter.irrIdeal();

  for (size_t i = 0; i < _varCount; ++i)
    t[i] = numeric_limits<Exponent>::max();

  Ideal::const_iterator stop = _subtract->end();
  for (Ideal::const_iterator it = _subtract->begin(); it != stop; ++it) {
    if (::getSizeOfSupport(*it, _varCount) != 1) {
      irreducible = false;
      break;
    }

    size_t var = ::getFirstNonZeroExponent(*it, _varCount);
    if ((*it)[var] < t[var])
      t[var] = (*it)[var];
  }

  if (irreducible)
    return _ideal->filter(filter);
  else {
    bool removedAny = false;
    for (Ideal::const_iterator it = _subtract->begin();
	 it != _subtract->end(); ++it)
      if (_ideal->removeStrictMultiples(*it))
	removedAny = true;
    return removedAny;
  }
}

bool SliceAlgorithm::Slice::filterDoubleLcm() {
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

SliceAlgorithm::Slice::Slice(Ideal* ideal,
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

SliceAlgorithm::Slice::Slice(Ideal* ideal,
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

SliceAlgorithm::Slice::Slice(Ideal* ideal,
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

SliceAlgorithm::Slice::~Slice() {
  delete _ideal;
  delete _subtract;
}

void SliceAlgorithm::Slice::addToSubtract(const Term& term) {
  _subtract->insert(term);
  _ideal->removeStrictMultiples(term);
}

const Term& SliceAlgorithm::Slice::getLcm() const {
  _ideal->getLcm(_lcm);
  return _lcm;
}

void SliceAlgorithm::Slice::print() const {
  cerr << "SLICE: Multiply: " << _multiply << endl;
  cerr << "Ideal: ";
  _ideal->print();

  cerr << "Subtract: ";
  _subtract->print();

  cerr << flush;
}
