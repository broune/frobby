#include "stdinc.h"
#include "SliceStrategy.h"

#include "Ideal.h"
#include <vector>

SliceStrategy::~SliceStrategy() {
}

void SliceStrategy::startingContent(const Slice& slice) {
}

void SliceStrategy::endingContent() {
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

class LabelSliceStrategy : public SliceStrategy {
  SplitType getSplitType(const Slice& slice) {
    return LabelSplit;
  }

  size_t getLabelSplitVariable(const Slice& slice) {
    Term co(slice.getVarCount());

    // TODO: This is duplicate code from PivotSliceStrategy. Factor
    // out into Slice.
    for (Ideal::const_iterator it = slice.getIdeal()->begin();
	 it != slice.getIdeal()->end(); ++it) {
      for (size_t var = 0; var < slice.getVarCount(); ++var)
	if ((*it)[var] > 0)
	  ++co[var];
    }

    return co.getFirstMaxExponent();
  }
}; 

class PivotSliceStrategy : public SliceStrategy {
  SplitType getSplitType(const Slice& slice) {
    return PivotSplit;
  }

  void getPivot(Term& pivot, const Slice& slice) {
    const Term& lcm = slice.getLcm();

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
}; 

SliceStrategy* SliceStrategy::newStrategy(const string& name) {
  if (name == "label")
    return new LabelSliceStrategy();
  else if (name == "pivot")
    return new PivotSliceStrategy();

  return 0;
}

class StatisticsSliceStrategy : public SliceStrategy {
public:
  StatisticsSliceStrategy(SliceStrategy* strategy):
    _strategy(strategy),
    _level (0) {
    ASSERT(strategy != 0);
  }

  ~StatisticsSliceStrategy() {
    delete _strategy;

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

    _strategy->startingContent(slice);
  }

  void endingContent() {
    --_level;

    _strategy->endingContent();
  }

  SplitType getSplitType(const Slice& slice) {
    return _strategy->getSplitType(slice);
  }

  void getPivot(Term& pivot, const Slice& slice) {
    _strategy->getPivot(pivot, slice);
  }

  size_t getLabelSplitVariable(const Slice& slice) {
    return _strategy->getLabelSplitVariable(slice);
  }

private:
  SliceStrategy* _strategy;

  size_t _level;
  vector<size_t> _calls;
};

SliceStrategy* SliceStrategy::addStatistics(SliceStrategy* strategy) {
  return new StatisticsSliceStrategy(strategy);
}

class DebugSliceStrategy : public SliceStrategy {
 public:
  DebugSliceStrategy(SliceStrategy* strategy):
    _strategy(strategy),
    _level(0) {
  }

  ~DebugSliceStrategy() {
    delete _strategy;
  }

  void startingContent(const Slice& slice) {
    ++_level;
    cerr << "DEBUG " << _level
	 << ": computing content of the following slice." << endl;
    slice.print();
    _strategy->startingContent(slice);
  }

  void endingContent() {
    cerr << "DEBUG " << _level
	 << ": done computing content of that slice." << endl;
    _strategy->endingContent();
    --_level;
  }

  SplitType getSplitType(const Slice& slice) {
    return _strategy->getSplitType(slice);
  }

  void getPivot(Term& pivot, const Slice& slice) {
    _strategy->getPivot(pivot, slice);
    cerr << "DEBUG " << _level
	 << ": performing pivot split on " << pivot << '.' << endl;
  }

  size_t getLabelSplitVariable(const Slice& slice) {
    size_t var = _strategy->getLabelSplitVariable(slice);
    cerr << "DEBUG " << _level
	 << ": performing label split on var " << var << '.' << endl;
    return var;
  }

private:
  SliceStrategy* _strategy;
  size_t _level;
};

SliceStrategy* SliceStrategy::addDebugOutput(SliceStrategy* strategy) {
  return new DebugSliceStrategy(strategy);
}
