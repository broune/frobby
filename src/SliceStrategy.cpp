#include "stdinc.h"
#include "SliceStrategy.h"

#include "Slice.h"
#include "Term.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include <vector>

SliceStrategy::~SliceStrategy() {
}

void SliceStrategy::doingIndependenceSplit(const Partition& partition) {
  cerr << "ERROR: doingIndependenceSplit not implemented." << endl;
  ASSERT(false);
}

void SliceStrategy::doingIndependentPart(const Partition& partition,
					 int setId) {
  cerr << "ERROR: doingIndependentPart not implemented." << endl;
  ASSERT(false);
}

void SliceStrategy::doneWithIndependentPart() {
  cerr << "ERROR: donWithIndependentPart not implemented." << endl;
  ASSERT(false);
}

void SliceStrategy::doneWithIndependenceSplit() {
  cerr << "ERROR: doingWithIndependenceSplit not implemented." << endl;
  ASSERT(false);
}

void SliceStrategy::startingContent(const Slice& slice) {
}

void SliceStrategy::endingContent() {
}

void SliceStrategy::simplify(Slice& slice) {
  slice.simplify();
}

void SliceStrategy::getPivot(Term& pivot, const Slice& slice) {
  ASSERT(false);
  cerr << "SliceStrategy::getPivot called but not defined." << endl;
  exit(1);
}

size_t SliceStrategy::getLabelSplitVariable(const Slice& slice) {
  ASSERT(false);
  cerr << "SliceStrategy::getLabelSplitVariable called but not defined."
       << endl;
  exit(1);
}

class DecomSliceStrategy : public SliceStrategy {
public:
  DecomSliceStrategy(DecomConsumer* consumer):
    _consumer(consumer) {
  }

  virtual ~DecomSliceStrategy() {
    delete _consumer;
  }

  virtual void consume(const Term& term) {
    ASSERT(_consumer != 0);
    _consumer->consume(term);
  }

private:
  DecomConsumer* _consumer;
};

class LabelSliceStrategy : public DecomSliceStrategy {
public:
  LabelSliceStrategy(DecomConsumer* consumer):
    DecomSliceStrategy(consumer) {
  }

  SplitType getSplitType(const Slice& slice) {
    return LabelSplit;
  }

  size_t getLabelSplitVariable(const Slice& slice) {
    Term co(slice.getVarCount());

    // TODO: This is duplicate code from PivotSliceStrategy. Factor
    // out into Slice.
    for (Ideal::const_iterator it = slice.getIdeal().begin();
	 it != slice.getIdeal().end(); ++it) {
      for (size_t var = 0; var < slice.getVarCount(); ++var)
	if ((*it)[var] > 0)
	  ++co[var];
    }

    return co.getFirstMaxExponent();
  }
}; 

class PivotSliceStrategy : public DecomSliceStrategy {
public:
  PivotSliceStrategy(DecomConsumer* consumer):
    DecomSliceStrategy(consumer) {
  }
  
  SplitType getSplitType(const Slice& slice) {
    return PivotSplit;
  }

  void getPivot(Term& pivot, const Slice& slice) {
    const Term& lcm = slice.getLcm();

    Term co(slice.getVarCount());

    for (Ideal::const_iterator it = slice.getIdeal().begin();
	 it != slice.getIdeal().end(); ++it) {
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
  }
};

SliceStrategy* SliceStrategy::newDecomStrategy(const string& name,
					       DecomConsumer* consumer) {
  if (name == "label")
    return new LabelSliceStrategy(consumer);
  else if (name == "pivot")
    return new PivotSliceStrategy(consumer);

  return 0;
}

// A decorator (pattern) for a SliceStrategy that does nothing. The
// purpose of this class is to act as a convenient base class for
// other decorators.
class DecoratorSliceStrategy : public SliceStrategy {
public:
  DecoratorSliceStrategy(SliceStrategy* strategy):
    _strategy(strategy) {
    ASSERT(strategy != 0);
  }

  virtual ~DecoratorSliceStrategy() {
    delete _strategy;
  }

  virtual void startingContent(const Slice& slice) {
    _strategy->startingContent(slice);
  }

  virtual void endingContent() {
    _strategy->endingContent();
  }

  virtual void simplify(Slice& slice) {
    _strategy->simplify(slice);
  }

  virtual SplitType getSplitType(const Slice& slice) {
    return _strategy->getSplitType(slice);
  }

  virtual void getPivot(Term& pivot, const Slice& slice) {
    _strategy->getPivot(pivot, slice);
  }

  virtual size_t getLabelSplitVariable(const Slice& slice) {
    return _strategy->getLabelSplitVariable(slice);
  }

  virtual void consume(const Term& term) {
    _strategy->consume(term);
  }

private:
  SliceStrategy* _strategy;
};

class FrobeniusSliceStrategy : public DecoratorSliceStrategy {
public:
  FrobeniusSliceStrategy(SliceStrategy* strategy,
			 const vector<mpz_class>& instance,
			 const TermTranslator* translator,
			 mpz_class& frobeniusNumber):
    DecoratorSliceStrategy(strategy),
    _instance(instance),
    _translator(translator),
    _frobeniusNumber(frobeniusNumber),
    _maxDegree(-1) {
    ASSERT(instance.size() == translator->getNames().getVarCount() + 1);
  }

  virtual ~FrobeniusSliceStrategy() {
    _frobeniusNumber = _maxDegree;
    for (size_t var = 0; var < _instance.size(); ++var)
      _frobeniusNumber -= _instance[var];
  }

  virtual void simplify(Slice& slice) {
    slice.simplify();

    // This is becase we have not yet handled independence splits.
    if (slice.getVarCount() != _translator->getNames().getVarCount())
      return;

    Term bound(slice.getVarCount());
    mpz_class degree;

    while (true) {
      getUpperBound(slice, bound);
      getDegree(bound, degree);

      if (degree <= _maxDegree) {
	slice.clear();
	break;
      }

      Term colon(slice.getVarCount());
      mpz_class degreeLess;
      mpz_class difference;
      for (size_t var = 0; var < slice.getVarCount(); ++var) {
	if (bound[var] == slice.getMultiply()[var])
	  continue;

	difference = 
	  _translator->getExponent(var, bound[var] + 1) -
	  _translator->getExponent(var, slice.getMultiply()[var] + 1);
	difference *= _instance[var + 1];

	degreeLess = degree - difference;

	if (degreeLess <= _maxDegree)
	  colon[var] = 1;
      }

      if (colon.isIdentity())
	break;

      slice.innerSlice(colon);
      slice.simplify();
    }
  }

  virtual void consume(const Term& term) {
    ASSERT(term.getVarCount() == _translator->getNames().getVarCount());

    mpz_class degree;
    getDegree(term, degree);
    if (_maxDegree < degree)
      _maxDegree = degree;
  }

private:
  void getDegree(const Term& term, mpz_class& degree) {
    degree = 0;
    for (size_t var = 0; var < term.getVarCount(); ++var)
      degree += _instance[var + 1] *
	_translator->getExponent(var, term[var] + 1);
  }

  void getUpperBound(const Slice& slice, Term& bound) {
    ASSERT(bound.getVarCount() == slice.getVarCount());

    bound.product(slice.getLcm(), slice.getMultiply());

    for (size_t var = 0; var < bound.getVarCount(); ++var)
      --bound[var];

    for (size_t var = 0; var < bound.getVarCount(); ++var) {
      if (bound[var] == _translator->getMaxId(var) - 1) {
	ASSERT(bound[var] > 0);
	--bound[var];
      }
    }
  }

  bool isPromising(const Term& bound) {
    mpz_class degree;
    getDegree(bound, degree);
    return degree > _maxDegree;
  }

  const vector<mpz_class> _instance;
  const TermTranslator* _translator;
  mpz_class& _frobeniusNumber;

  mpz_class _maxDegree;
};

SliceStrategy* SliceStrategy::
newFrobeniusStrategy(const string& name,
		     const vector<mpz_class>& instance,
		     const TermTranslator* translator,
		     mpz_class& frobeniusNumber) {
  SliceStrategy* strategy = newDecomStrategy(name, 0);
  if (strategy == 0)
    return 0;

  return new FrobeniusSliceStrategy(strategy, instance,
				    translator, frobeniusNumber);
}

class StatisticsSliceStrategy : public DecoratorSliceStrategy {
public:
  StatisticsSliceStrategy(SliceStrategy* strategy):
    DecoratorSliceStrategy(strategy),
    _level (0) {
  }

  ~StatisticsSliceStrategy() {
    cerr << "**** Statistics" << endl;
    size_t sum = 0;
    for (size_t l = 0; l < _calls.size(); ++l) {
      sum += _calls[l];
      cerr << "Level " << l + 1 << " had "
	   << _calls[l] << " calls. " << endl;
    }
    cerr << endl << "Which makes for " << sum << " calls in all" << endl;
  }

  void startingContent(const Slice& slice) {
    if (_calls.size() == _level)
      _calls.push_back(0);
    ++_calls[_level];
    ++_level;

    DecoratorSliceStrategy::startingContent(slice);
  }

  void endingContent() {
    --_level;

    DecoratorSliceStrategy::endingContent();
  }

private:
  size_t _level;
  vector<size_t> _calls;
};

SliceStrategy* SliceStrategy::addStatistics(SliceStrategy* strategy) {
  return new StatisticsSliceStrategy(strategy);
}

class DebugSliceStrategy : public DecoratorSliceStrategy {
 public:
  DebugSliceStrategy(SliceStrategy* strategy):
    DecoratorSliceStrategy(strategy),
    _level(0) {
  }

  void startingContent(const Slice& slice) {
    ++_level;
    cerr << "DEBUG " << _level
	 << ": computing content of the following slice." << endl
	 << slice << endl;

    DecoratorSliceStrategy::startingContent(slice);
  }

  void endingContent() {
    cerr << "DEBUG " << _level
	 << ": done computing content of that slice." << endl;
    --_level;

    DecoratorSliceStrategy::endingContent();
  }

  void getPivot(Term& pivot, const Slice& slice) {
    DecoratorSliceStrategy::getPivot(pivot, slice);
    cerr << "DEBUG " << _level
	 << ": performing pivot split on " << pivot << '.' << endl;
  }

  size_t getLabelSplitVariable(const Slice& slice) {
    size_t var = DecoratorSliceStrategy::getLabelSplitVariable(slice);
    cerr << "DEBUG " << _level
	 << ": performing label split on var " << var << '.' << endl;
    return var;
  }

  void consume(const Term& term) {
    cerr << "DEBUG " << _level
	 << ": Writing " << term << " to output." << endl;
    DecoratorSliceStrategy::consume(term);
  }

private:
  size_t _level;
};

SliceStrategy* SliceStrategy::addDebugOutput(SliceStrategy* strategy) {
  return new DebugSliceStrategy(strategy);
}
