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
#include "Slice.h"
#include "DecomConsumer.h"

// TODO: move
unsigned int level = 0;
vector<unsigned int> calls;

class DecomWriter : public DecomConsumer {
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

class DecomStorer : public DecomConsumer {
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

// Requires slice.getIdeal() to be minimized.
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
  if (setCount == 1)
    return false;

  size_t at1 = 0;
  size_t at2 = 0;
  size_t over2 = 0;
  for (size_t i = 0; i < setCount; ++i) {
    size_t size = partition.getSetSize(i);
    if (size == 1)
      ++at1;
    else if (size == 2)
      ++at2;
    else
      ++over2;
  }

  if (at1 <= 2 && at2 == 0 && over2 <= 1 && slice.getIdeal()->getGeneratorCount() < 15)
    return false;
  
  vector<ProjDecom> projDecoms;
  projDecoms.reserve(setCount);

  Term term(slice.getVarCount());
  if (at1 > 0) {
    // This relies on slice.getIdeal() to be minimized.
    term = slice.getLcm();
    for (size_t i = 0; i < slice.getVarCount(); ++i)
      if (term[i] > 0)
	term[i] = slice.getMultiply()[i] + (term[i] - 1);
  }

  for (size_t i = 0; i < setCount; ++i) {
    size_t projVarCount = partition.getSetSize(i);
    if (projVarCount == 1)
      continue;

    projDecoms.resize(projDecoms.size() + 1);
    ProjDecom& projDecom = projDecoms.back();
    vector<Exponent>& compressor = projDecom.compressor;

    partition.getProjection(i, compressor);

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
    projDecom.decom = slice.getIdeal()->createNew(projVarCount);
    _decomConsumer = new DecomStorer(projDecom.decom);
    
    content(projSlice, true);
    
    delete _decomConsumer;
    _decomConsumer = oldConsumer;
  }

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
    inner.normalize();
    content(inner);
  }

  {
    Slice outer(slice.getIdeal()->clone(),
		slice.getSubtract()->clone(),
		slice.getMultiply());

    outer.getIdeal()->removeStrictMultiples(pivot);
    if (pivot.getSizeOfSupport() > 1)
      outer.getSubtract()->insert(pivot);
    
    content(outer);
  }
}

void SliceAlgorithm::content(Slice& slice, bool simplifiedAndDependent) {
  if (!simplifiedAndDependent)
    slice.simplify();
  
  if (calls.size() == level)
    calls.push_back(0);
  ++calls[level];
  ++level;

  if (!slice.baseCase(_decomConsumer)) {
    if (simplifiedAndDependent || !independenceSplit(slice)) {
      if (true)
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
  const Term& lcm = slice.getLcm();
  //  size_t maxOffset = lcm.getFirstMaxExponent();

  Term co(slice.getVarCount());

  for (Ideal::const_iterator it = slice.getIdeal()->begin();
       it != slice.getIdeal()->end(); ++it) {
    for (size_t var = 0; var < slice.getVarCount(); ++var)
      if ((*it)[var] > 0)
	++co[var];
  }

  size_t maxOffset;
  do {
    maxOffset = co.getFirstMaxExponent();
    co[maxOffset] = 0;
  } while (lcm[maxOffset] <= 1);

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
