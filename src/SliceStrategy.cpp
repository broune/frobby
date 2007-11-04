#include "stdinc.h"
#include "SliceStrategy.h"

#include "Ideal.h"
#include <vector>

SliceStrategy::~SliceStrategy() {
}

void SliceStrategy::startingContent(const Slice& slice) {
}

void SliceStrategy::endingContent(const Slice& slice) {
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
    return slice.getLcm().getFirstMaxExponent();
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

SliceStrategy* SliceStrategy::newStrategy(const char* name) {
  string str(name);
  if (str == "label")
    return new LabelSliceStrategy();
  else if (str == "pivot")
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

  void endingContent(const Slice& slice) {
    --_level;

    _strategy->endingContent(slice);
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
