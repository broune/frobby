/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "MsmStrategy.h"

#include "MsmSlice.h"
#include "Term.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include <vector>
#include "Projection.h"
#include "SliceAlgorithm.h"
#include "TermGrader.h"
#include "IndependenceSplitter.h"
#include "SliceEvent.h"

Slice* MsmStrategy::setupInitialSlice(const Ideal& ideal) {
  size_t varCount = ideal.getVarCount();

  Term sliceMultiply(varCount);
  for (size_t var = 0; var < varCount; ++var)
	sliceMultiply[var] = 1;

  Slice* slice = new MsmSlice(ideal,
							  Ideal(varCount),
							  sliceMultiply,
							  this);
  simplify(*slice);
  initialize(*slice);

  return slice;
}

MsmSlice* MsmStrategy::newMsmSlice() {
  Slice* slice = newSlice();
  ASSERT(dynamic_cast<MsmSlice*>(slice) != 0);
  return static_cast<MsmSlice*>(slice);
}

Slice* MsmStrategy::allocateSlice() {
  return new MsmSlice();
}

bool MsmStrategy::debugIsValidSlice(Slice* slice) {
  ASSERT(slice != 0);
  ASSERT(dynamic_cast<MsmSlice*>(slice) != 0);
  return true;
}

void MsmStrategy::labelSplit(Slice* slice,
							 Slice*& leftSlice, Slice*& rightSlice) {
  ASSERT(!slice->normalize());
  size_t var = getLabelSplitVariable(*slice);

  Term term(slice->getVarCount());

  const Term& lcm = slice->getLcm();

  Ideal::const_iterator stop = slice->getIdeal().end();
  Ideal::const_iterator label = stop;
  bool hasTwoLabels = false;
  for (Ideal::const_iterator it = slice->getIdeal().begin();
	   it != stop; ++it) {
    if ((*it)[var] == 1) {
	  term = *it;
	  term[var] -= 1;

	  bool couldBeLabel = !slice->getSubtract().contains(term);
	  if (couldBeLabel) {
		for (size_t v = 0; v < slice->getVarCount(); ++v) {
		  if (term[v] == lcm[v]) {
			couldBeLabel = false;
			break;
		  }
		}
	  }

	  if (couldBeLabel) {
		if (label == stop)
		  label = it;
		else {
		  hasTwoLabels = true;
		  break;
		}
	  }
	}
  }

  Slice* hasLabelSlice = 0;

  if (label != stop) {
	term = *label;
	term[var] -= 1;

	hasLabelSlice = newSlice();
	*hasLabelSlice = *slice;
	hasLabelSlice->innerSlice(term);

	if (hasTwoLabels)
	  slice->outerSlice(term);
  }

  if (!hasTwoLabels) {
	term.setToIdentity();
	term[var] = 1;
	slice->innerSlice(term);
  }

  leftSlice = hasLabelSlice;
  rightSlice = slice;
}

MsmStrategy::MsmStrategy():
  _useIndependence(true) {
}

MsmStrategy::~MsmStrategy() {
}

class IndepEvents {
public:
  SliceEvent* getLeftEvent() {
	return &_leftEvent;
  }

  SliceEvent* getRightEvent() {
	return &_rightEvent;
  }

  const Projection& getLeftProjection() {
	return _leftProjection;
  }

  const Projection& getRightProjection() {
	return _rightProjection;
  }

  static IndepEvents* newEvents(MsmStrategy* strategy,
								const Slice& slice,
								IndependenceSplitter& splitter) {
	return new IndepEvents(strategy, slice, splitter);
  }

private:
  IndepEvents(MsmStrategy* strategy,
			  const Slice& slice,
			  IndependenceSplitter& splitter):
	_strategy(strategy),
	_leftEvent(this),
	_rightEvent(this) {
	splitter.getBigProjection(_leftProjection);
	splitter.getRestProjection(_rightProjection);

	_strategy->doingIndependenceSplit(slice, splitter);
	_strategy->doingIndependentPart(_rightProjection, false);
  }

  void raiseLeftEvent() {
	_strategy->doneWithIndependenceSplit();
	delete this;
  }

  void raiseRightEvent() {
	_strategy->doingIndependentPart(_leftProjection, true);
  }

  class LeftEvent : public SliceEvent {
  public:
	LeftEvent(IndepEvents* events):
	  _events(events) {
	}

	virtual void raiseEvent() {
	  _events->raiseLeftEvent();
	}

  private:
	IndepEvents* _events;
  };

  class RightEvent : public SliceEvent {
  public:
	RightEvent(IndepEvents* events):
	  _events(events) {
	}

	virtual void raiseEvent() {
	  _events->raiseRightEvent();
	}

	IndepEvents* _events;
  };

  MsmStrategy* _strategy;

  LeftEvent _leftEvent;
  RightEvent _rightEvent;

  // have to have two projections, as the strategy keeps a reference
  // to the projection rather than copy it or something like that.
  // TODO: change (=fix) this.
  Projection _leftProjection;
  Projection _rightProjection;
};

bool MsmStrategy::independenceSplit
(MsmSlice* slice,
 SliceEvent*& leftEvent, Slice*& leftSlice,
 SliceEvent*& rightEvent, Slice*& rightSlice) {

  static IndependenceSplitter indep;
  if (!indep.analyze(*slice))
    return false;

  IndepEvents* events = IndepEvents::newEvents(this, *slice, indep);

  leftEvent = events->getLeftEvent();
  MsmSlice* msmLeftSlice = new MsmSlice();
  msmLeftSlice->setToProjOf(*slice, events->getLeftProjection(), this);
  leftSlice = msmLeftSlice;

  rightEvent = events->getRightEvent();
  MsmSlice* msmRightSlice = new MsmSlice();
  msmRightSlice->setToProjOf(*slice, events->getRightProjection(), this);
  rightSlice = msmRightSlice;

  freeSlice(slice);

  return true;
}

void MsmStrategy::split(Slice* sliceParam,
						SliceEvent*& leftEvent, Slice*& leftSlice,
						SliceEvent*& rightEvent, Slice*& rightSlice) {
  ASSERT(sliceParam != 0);
  ASSERT(dynamic_cast<MsmSlice*>(sliceParam) != 0);
  MsmSlice* slice = (MsmSlice*)sliceParam;

  ASSERT(leftEvent == 0);
  ASSERT(leftSlice == 0);
  ASSERT(rightEvent == 0);
  ASSERT(rightSlice == 0);

  if (_useIndependence &&
	  independenceSplit(slice,
						leftEvent, leftSlice,
						rightEvent, rightSlice))
	return;

  switch (getSplitType(*slice)) {
  case MsmStrategy::LabelSplit:
	labelSplit(slice, leftSlice, rightSlice);
	break;

  case MsmStrategy::PivotSplit:
	pivotSplit(slice, leftSlice, rightSlice);
	break;

  default:
	ASSERT(false);
  }

  if (leftSlice != 0)
	simplify(*leftSlice);

  if (rightSlice != 0)
	simplify(*rightSlice);
}

void MsmStrategy::initialize(const Slice& slice) {
}

void MsmStrategy::simplify(Slice& slice) {
  slice.simplify();
}

void MsmStrategy::getPivot(Term& pivot, Slice& slice) {
  fputs("INTERNAL ERROR: Undefined MsmStrategy::getPivot called.\n", stderr);
  ASSERT(false);
  exit(1);
}

size_t MsmStrategy::getLabelSplitVariable(const Slice& slice) {
  fputs
    ("INTERNAL ERROR: Undefined MsmStrategy::getLabelSplitVariable called.\n",
     stderr);
  ASSERT(false);
  exit(1);
}

class DecomMsmStrategy : public MsmStrategy {
public:
  DecomMsmStrategy(TermConsumer* consumer):
    _consumer(consumer) {
  }

  virtual ~DecomMsmStrategy() {
    ASSERT(_independenceSplits.empty());
    delete _consumer;
  }

  virtual void consume(const Term& term) {
    ASSERT(getCurrentConsumer() != 0);

    getCurrentConsumer()->consume(term);
  }

  virtual void doingIndependenceSplit(const Slice& slice,
									  IndependenceSplitter& splitter) {
    _independenceSplits.push_back
      (new IndependenceSplit(splitter,
							 getCurrentConsumer(),
							 slice.getVarCount()));
  }
 
  virtual void doingIndependentPart(const Projection& projection, bool last) {
    getCurrentSplit()->setCurrentPart(projection, last);
  }

  virtual void doneWithIndependenceSplit() {
    delete getCurrentSplit();
    _independenceSplits.pop_back();
  }

private:
  class IndependenceSplit : public TermConsumer {
  public:
    IndependenceSplit(IndependenceSplitter& splitter,
					  TermConsumer* consumer,
					  size_t varCount):
	  _partialTerm(varCount),
      _consumer(consumer) {
      ASSERT(_consumer != 0);

	  _right = true;
	  splitter.getBigProjection(_leftProjection);
	  splitter.getRestProjection(_rightProjection);

	  _rightDecom.clearAndSetVarCount(_rightProjection.getRangeVarCount());
    }

    virtual void consume(const Term& term) {
      if (_right)
		consumeRight(term);
	  else
		consumeLeft(term);
    }

	void consumeLeft(const Term& term) {
	  _leftProjection.inverseProject(_partialTerm, term);
      Ideal::const_iterator stop = _rightDecom.end();
      for (Ideal::const_iterator it = _rightDecom.begin(); it != stop; ++it) {
		_rightProjection.inverseProject(_partialTerm, *it);
		_consumer->consume(_partialTerm);
      }
	}

	void consumeRight(const Term& term) {
	  _rightDecom.insert(term);
	}

    void setCurrentPart(const Projection& projection, bool last) {
	  _right = !last;
    }

  private:
    Projection _leftProjection;
	Projection _rightProjection;
	Ideal _rightDecom;

    Term _partialTerm;

    TermConsumer* _consumer;

	bool _right;
  };

  IndependenceSplit* getCurrentSplit() {
    ASSERT(!_independenceSplits.empty());
    ASSERT(_independenceSplits.back() != 0);
    return _independenceSplits.back();
  }

  TermConsumer* getCurrentConsumer() {
    if (_independenceSplits.empty())
      return _consumer;
    else
      return getCurrentSplit();
  }

  vector<IndependenceSplit*> _independenceSplits;
  TermConsumer* _consumer;
};

class LabelMsmStrategy : public DecomMsmStrategy {
public:
  enum Type {
	MaxLabel,
	MinLabel,
    VarLabel
  };

  LabelMsmStrategy(Type type, TermConsumer* consumer):
    DecomMsmStrategy(consumer),
	_type(type) {
  }

  SplitType getSplitType(const Slice& slice) {
    return LabelSplit;
  }

  size_t getLabelSplitVariable(const Slice& slice) {
	Term co(slice.getVarCount());
	slice.getIdeal().getSupportCounts(co);

	if (_type == MaxLabel) {
	  // Return the variable that divides the most minimal generators.
	  // This cannot be an invalid split because if every count was 1,
	  // then this would be a base case.
	  return co.getFirstMaxExponent();
	}

	// For each variable, count number of terms with exponent of 1
	Term co1(slice.getVarCount());
	Ideal::const_iterator end = slice.getIdeal().end();
	for (Ideal::const_iterator it = slice.getIdeal().begin();
		 it != end; ++it) {
	  // This way we avoid bad splits.
	  if (getSizeOfSupport(*it, slice.getVarCount()) == 1)
		continue;
	  for (size_t var = 0; var < slice.getVarCount(); ++var)
		if ((*it)[var] == 1)
		  co1[var] += 1;
	}

	// The slice is simplified and not a base case slice.
	ASSERT(!co1.isIdentity());

	if (_type == VarLabel) {
	  // Return the least variable that is valid.
	  for (size_t var = 0; ; ++var) {
		ASSERT(var < slice.getVarCount());
		if (co1[var] > 0)
		  return var;
	  }
	}

	if (_type != MinLabel) {
	  fputs("INTERNAL ERROR: Undefined label split type.\n", stderr);
	  ASSERT(false);
	  exit(1);
	}

	// Zero those variables of co that have more than the least number
	// of exponent 1 minimal generators.
	size_t mostGeneric = 0;
	for (size_t var = 1; var < slice.getVarCount(); ++var)
	  if (mostGeneric == 0 || (mostGeneric > co1[var] && co1[var] > 0))
		mostGeneric = co1[var];
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  if (co1[var] != mostGeneric)
		co[var] = 0;

	// Among those with least exponent 1 minimal generators, return
	// the variable that divides the most minimal generators.
	return co.getFirstMaxExponent();
  }

  private:
	Type _type;
}; 

class PivotMsmStrategy : public DecomMsmStrategy {
public:
  PivotMsmStrategy(PivotStrategy pivotStrategy, TermConsumer* consumer):
    DecomMsmStrategy(consumer),
    _pivotStrategy(pivotStrategy) {
	srand(0); // to make things repeatable
  }

  SplitType getSplitType(const Slice& slice) {
    return PivotSplit;
  }

  void getPivot(Term& pivot, Slice& slice) {
	SliceStrategyCommon::getPivot(pivot, slice, _pivotStrategy);
	return;
  }

private:
  PivotStrategy _pivotStrategy;
};

MsmStrategy* MsmStrategy::newDecomStrategy(const string& name,
										   TermConsumer* consumer) {
  if (name == "maxlabel")
    return new LabelMsmStrategy(LabelMsmStrategy::MaxLabel, consumer);
  else if (name == "minlabel")
    return new LabelMsmStrategy(LabelMsmStrategy::MinLabel, consumer);
  else if (name == "varlabel")
    return new LabelMsmStrategy(LabelMsmStrategy::VarLabel, consumer);

  SliceStrategyCommon::PivotStrategy pivotStrategy =
	SliceStrategyCommon::getPivotStrategy(name);

  if (pivotStrategy == SliceStrategyCommon::Unknown) {
	fprintf(stderr, "ERROR: Unknown split strategy \"%s\".\n", name.c_str());
	exit(1);
  }

  return new PivotMsmStrategy(pivotStrategy, consumer);
}

class FrobeniusIndependenceSplit : public TermConsumer {
public:
  FrobeniusIndependenceSplit(const TermGrader& grader, bool useBound):
    _grader(grader),
    _bound(grader.getVarCount()),
    _toBeat(-1),
    _improved(true),
    _partValue(-1), 
    _partProjection(&_projection),
	_useBound(useBound) {
    _projection.setToIdentity(grader.getVarCount());
    _outerPartProjection = _projection;
  }

  FrobeniusIndependenceSplit(const Projection& projection,
							 const Slice& slice,
							 const mpz_class& toBeat,
							 const TermGrader& grader,
							 bool useBound):
    _grader(grader),
    _bound(slice.getVarCount()),
    _toBeat(toBeat),
    _improved(true),
    _projection(projection),
	_useBound(useBound) {
	if (useBound)
	  getUpperBound(slice, _bound);
  }

  virtual void consume(const Term& term) {
    getDegree(term, _outerPartProjection, _tmpInt);

    if (_tmpInt > _partValue) {
      _partProjection->inverseProject(_bound, term);
      _partValue = _tmpInt;
      _improved = true;
    }
  }

  const Term& getBound() const {
    return _bound;
  }

  bool hasImprovement() const {
    return _improved;
  }

  const mpz_class& getCurrentPartValue() const {
    return _partValue;
  }

  void getBoundDegree(mpz_class& degree) {
    getDegree(_bound, _projection, degree);
  }

  void setCurrentPart(const Projection& projection) {
    _improved = false;
    _partProjection = &projection;

    updateOuterPartProjection();

	if (_useBound) {
	  Term zero(_partProjection->getRangeVarCount());
	  _partProjection->inverseProject(_bound, zero);
	  getDegree(zero, _outerPartProjection, _partValue);
	  
	  if (_toBeat == -1)
		_partValue = -1;
	  else {
		getDegree(_bound, _projection, _tmpInt);
		_partValue = _toBeat - (_tmpInt - _partValue);
	  }
	} else
	  _partValue = -1;
  }

  void getPivot(Term& pivot, Slice& slice) {
    const Term& lcm = slice.getLcm();

	mpz_class maxDiff;
	mpz_class diff;

	maxDiff = -1;
    size_t maxOffset = (size_t)-1;
	for (size_t var = 0; var < slice.getVarCount(); ++var) {
	  if (lcm[var] <= 1)
		continue;

	  size_t outerVar = _outerPartProjection.inverseProjectVar(var);
	  Exponent e = lcm[var] / 2;
	  diff =
		_grader.getGrade(outerVar, slice.getMultiply()[var] + e) -
		_grader.getGrade(outerVar, slice.getMultiply()[var]);
	  ASSERT(diff >= 0);

	  if (diff > maxDiff) {
		maxOffset = var;
		maxDiff = diff;
	  }
	}
	ASSERT(maxOffset != (size_t)-1);

    pivot.setToIdentity();
	pivot[maxOffset] = lcm[maxOffset] / 2;
  }

  const Projection& getCurrentPartProjection() {
    return _outerPartProjection;
  }

  void simplify(Slice& slice) {
    if (slice.getIdeal().getGeneratorCount() == 0)
      return;

    if (_partValue == -1 || !_useBound) {
      slice.simplify();
      return;
    }

	Term bound(slice.getVarCount());
	Term oldBound(slice.getVarCount());
	Term colon(slice.getVarCount());
	getUpperBound(slice, bound);

	while (true) {
	  // Obtain bound for degree
	  mpz_class degree;
	  getDegree(bound, _outerPartProjection, degree);

	  // Check if improvement is possible
	  if (degree <= _partValue) {
		slice.clear();
		break;
	  }

	  // Use above bound to obtain improved lower bound. The idea is
	  // to consider artinian pivots and to rule out the outer slice
	  // using the above condition. If this can be done, then we can
	  // perform the split and ignore the outer slice.
	  for (size_t var = 0; var < slice.getVarCount(); ++var)
		colon[var] =
		  improveLowerBound(var, degree, bound, slice.getMultiply());

	  // Check if any improvement were made.
	  oldBound = bound;
	  if (!colon.isIdentity()) {
		slice.innerSlice(colon);
		getUpperBound(slice, bound);
		if (bound != oldBound)
		  continue; // Iterate process using new bound.
	  }

	  slice.simplify();
	  if (slice.getIdeal().getGeneratorCount() == 0)
		break;

	  getUpperBound(slice, bound);
	  if (bound == oldBound)
		break;

	  // Iterate process using new bound.
	}
  }

private:
  Exponent improveLowerBound(size_t var,
							 const mpz_class& upperBoundDegree,
							 const Term& upperBound,
							 const Term& lowerBound) {
	if (upperBound[var] == lowerBound[var])
	  return 0;

	size_t outerVar = _outerPartProjection.inverseProjectVar(var);
	mpz_class baseUpperBoundDegree;
	baseUpperBoundDegree = upperBoundDegree -
	  _grader.getGrade(outerVar, upperBound[var]);

	// Exponential search followed by binary search.
	Exponent low = 0;
	Exponent high = upperBound[var] - lowerBound[var];

	while (low < high) {
	  Exponent mid;
	  if (low < high - low)
		mid = 2 * low;
	  else {
		// This way of expressing (low + high) / 2 avoids the
		// possibility of low + high causing an overflow.
		mid = low + (high - low) / 2;
	  }

	  _tmpInt = baseUpperBoundDegree +
		_grader.getGrade(outerVar, lowerBound[var] + mid);

	  if (_tmpInt <= _partValue)
		low = mid + 1;
	  else
		high = mid;
	}
	ASSERT(low == high);

	return low;
  }

  void updateOuterPartProjection() {
    vector<size_t> inverses;
    for (size_t var = 0; var < _partProjection->getRangeVarCount(); ++var) {
      size_t middleVar = _partProjection->inverseProjectVar(var);
      size_t outerVar = _projection.inverseProjectVar(middleVar);
      
      inverses.push_back(outerVar);
    }

    _outerPartProjection.reset(inverses);
  }

  void getUpperBound(const Slice& slice, Term& bound) {
    ASSERT(bound.getVarCount() == slice.getVarCount());
    bound = slice.getLcm();
    adjustToBound(slice, bound);
  }

  void adjustToBound(const Slice& slice, Term& bound) {
    ASSERT(bound.getVarCount() == slice.getVarCount());

    bound.product(bound, slice.getMultiply());

    for (size_t var = 0; var < bound.getVarCount(); ++var) {
      ASSERT(bound[var] > 0);
      --bound[var];
    }

    for (size_t var = 0; var < bound.getVarCount(); ++var)
      if (bound[var] == _grader.getMaxExponent(var) &&
		  slice.getMultiply()[var] < bound[var])
		--bound[var];
  }

  void getDegree(const Term& term,
				 const Projection& projection,
				 mpz_class& degree) {
    _grader.getDegree(term, projection, degree);
  }

  const TermGrader& _grader;

  Term _bound;
  mpz_class _toBeat;
  bool _improved;

  mpz_class _partValue;
  const Projection* _partProjection;
  Projection _outerPartProjection;

  Projection _projection;

  bool _useBound;
  mpz_class _tmpInt;
};

// Ignores the possibility of "fake" artinians (e.g. in getPivot).
//
// The passed-in strategy is only used as a split strategy, and it
// does NOT get informed about anything other than pivot and label
// split requests. If it is null, then a specialized grade-base pivot
// selection algorithm is used.
class FrobeniusMsmStrategy : public MsmStrategy {
public:
  FrobeniusMsmStrategy(MsmStrategy* strategy,
					   TermConsumer* consumer,
					   TermGrader& grader,
					   bool useBound):
	_strategy(strategy),
    _consumer(consumer),
    _grader(grader),
	_useBound(useBound) {
    ASSERT(_consumer != 0);

    _independenceSplits.push_back
      (new FrobeniusIndependenceSplit(_grader, _useBound));
  }

  virtual ~FrobeniusMsmStrategy() {
    ASSERT(_consumer != 0);
    _consumer->consume(getCurrentSplit()->getBound());

    delete getCurrentSplit();
    _independenceSplits.pop_back();
    ASSERT(_independenceSplits.empty());
  }

  virtual void initialize(const Slice& slice) {
    // The purpose of this is to initialize the bound so that it can
    // be used right away, instead of waiting for the slice algorithm
    // to produce some output first.
	if (_useBound) {
	  Term msm;
	  if (computeSingleMSM(slice, msm))
		consume(msm);
	}
  }

  virtual void simplify(Slice& slice) {
    getCurrentSplit()->simplify(slice);
  }

  virtual void consume(const Term& term) {
    getCurrentSplit()->consume(term);
  }

  virtual SplitType getSplitType(const Slice& slice) {
	if (_strategy == 0)
	  return PivotSplit;
	else
	  return _strategy->getSplitType(slice);
  }

  virtual void getPivot(Term& pivot, Slice& slice) {
	if (_strategy != 0)
	  _strategy->getPivot(pivot, slice);
	else
	  getCurrentSplit()->getPivot(pivot, slice);
  }

  virtual size_t getLabelSplitVariable(const Slice& slice) {
	ASSERT(_strategy != 0);
	return _strategy->getLabelSplitVariable(slice);
  }

  virtual void doingIndependenceSplit(const Slice& slice,
									  IndependenceSplitter& splitter) {
	_independenceSplits.push_back
      (new FrobeniusIndependenceSplit
       (getCurrentSplit()->getCurrentPartProjection(),
		slice,
		getCurrentSplit()->getCurrentPartValue(),
		_grader,
		_useBound));
  }
  
  virtual void doingIndependentPart(const Projection& projection, bool last) {
    getCurrentSplit()->setCurrentPart(projection);
  }
  
  virtual void doneWithIndependenceSplit() {
    FrobeniusIndependenceSplit* oldSplit = getCurrentSplit();
    _independenceSplits.pop_back();

    if (oldSplit->hasImprovement())
      consume(oldSplit->getBound());
    delete oldSplit;
  }

private:
  FrobeniusIndependenceSplit* getCurrentSplit() {
    ASSERT(!_independenceSplits.empty());
    return _independenceSplits.back();
  }

  MsmStrategy* _strategy;
  TermConsumer* _consumer;
  vector<FrobeniusIndependenceSplit*> _independenceSplits;
  const TermGrader& _grader;
  bool _useBound;
};

MsmStrategy* MsmStrategy::
newFrobeniusStrategy(const string& name,
					 TermConsumer* consumer,
					 TermGrader& grader,
					 bool useBound) {
  MsmStrategy* strategy;
  if (name == "frob")
	strategy = 0;
  else
	strategy = newDecomStrategy(name, 0);

  return new FrobeniusMsmStrategy(strategy, consumer, grader, useBound);
}

MsmStrategy* MsmStrategy::addStatistics(MsmStrategy* strategy) {
  fputs("INTERNAL ERROR: statistics are awaiting reimplementation.", stderr);
  ASSERT(false);
  exit(1);
}

MsmStrategy* MsmStrategy::addDebugOutput(MsmStrategy* strategy) {
  fputs("INTERNAL ERROR: debug is awaiting reimplementation.", stderr);
  ASSERT(false);
  exit(1);
}
