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

MsmSlice* MsmStrategy::setupInitialSlice(const Ideal& ideal) {
  size_t varCount = ideal.getVarCount();

  Term sliceMultiply(varCount);
  for (size_t var = 0; var < varCount; ++var)
	sliceMultiply[var] = 1;

  MsmSlice* slice = new MsmSlice(ideal, Ideal(varCount), sliceMultiply);
  simplify(*slice);
  initialize(*slice);

  return slice;
}

void MsmStrategy::freeSlice(MsmSlice* slice) {
  ASSERT(slice != 0);

  slice->clear(); // To preserve memory.
  _sliceCache.push_back(slice);
}

MsmSlice* MsmStrategy::newSlice() {
  if (_sliceCache.empty())
	return new MsmSlice();

  MsmSlice* slice = _sliceCache.back();
  _sliceCache.pop_back();
  return slice;
}

pair<MsmSlice*, MsmSlice*> MsmStrategy::labelSplit(MsmSlice* slice) {
  ASSERT(!slice->normalize());
  size_t var = getLabelSplitVariable(*slice);

  Term term(slice->getVarCount());

  const Term& lcm = slice->getLcm();

  Ideal::const_iterator stop = slice->getIdeal().end();
  Ideal::const_iterator label = stop;
  bool hasTwoLabels = false;
  for (Ideal::const_iterator it = slice->getIdeal().begin(); it != stop; ++it) {
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

  MsmSlice* hasLabelSlice = 0;

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

  return make_pair(hasLabelSlice, slice);
}

pair<MsmSlice*, MsmSlice*> MsmStrategy::pivotSplit(MsmSlice* slice) {
  Term pivot(slice->getVarCount());
  getPivot(pivot, *slice);

  ASSERT(!pivot.isIdentity()); 
  ASSERT(!slice->getIdeal().contains(pivot));
  ASSERT(!slice->getSubtract().contains(pivot));

  MsmSlice* inner = newSlice();
  *inner = *slice;
  inner->innerSlice(pivot);

  slice->outerSlice(pivot);

  return make_pair(inner, slice);
}

MsmStrategy::~MsmStrategy() {
}

pair<MsmSlice*, MsmSlice*> MsmStrategy::split(MsmSlice* slice) {
  pair<MsmSlice*, MsmSlice*> slicePair;

  switch (getSplitType(*slice)) {
  case MsmStrategy::LabelSplit:
	slicePair = labelSplit(slice);
	break;

  case MsmStrategy::PivotSplit:
	slicePair = pivotSplit(slice);
	break;

  default:
	ASSERT(false);
  }

  if (slicePair.first != 0)
	simplify(*slicePair.first);

  if (slicePair.second != 0)
	simplify(*slicePair.second);

  return slicePair;
}

void MsmStrategy::initialize(const MsmSlice& slice) {
}

void MsmStrategy::simplify(MsmSlice& slice) {
  slice.simplify();
}

void MsmStrategy::getPivot(Term& pivot, MsmSlice& slice) {
  fputs("INTERNAL ERROR: Undefined MsmStrategy::getPivot called.\n", stderr);
  ASSERT(false);
  exit(1);
}

size_t MsmStrategy::getLabelSplitVariable(const MsmSlice& slice) {
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

  virtual void doingIndependenceSplit(const MsmSlice& slice,
									  Ideal* mixedProjectionSubtract) {
    _independenceSplits.push_back
      (new IndependenceSplit(slice,
							 mixedProjectionSubtract,
							 getCurrentConsumer()));
  }
 
  virtual void doingIndependentPart(const Projection& projection, bool last) {
    getCurrentSplit()->setCurrentPart(projection, last);
  }

  virtual bool doneWithIndependentPart() {
    return getCurrentSplit()->doneWithPart();
  }

  virtual void doneWithIndependenceSplit() {
    delete getCurrentSplit();
    _independenceSplits.pop_back();
  }

private:
  class IndependenceSplit : public TermConsumer {
  public:
    IndependenceSplit(const MsmSlice& slice,
					  Ideal* mixedProjectionSubtract,
					  TermConsumer* consumer):
      _partialTerm(slice.getMultiply()),
      _mixedProjectionSubtract(mixedProjectionSubtract),
      _consumer(consumer),
      _lastPartProjection(0) {
      ASSERT(_consumer != 0);
    }

    ~IndependenceSplit() {
    }

    virtual void consume(const Term& term) {
      if (_lastPartProjection != 0) {
		_lastPartProjection->inverseProject(_partialTerm, term);
		generateDecom();
      } else
		_parts.back().decom.insert(term);
    }

    // Must set each part exactly once, and must call doneWithPart
    // after having called setCurrentPart().
    bool setCurrentPart(const Projection& projection, bool last) {
      ASSERT(_lastPartProjection == 0);

      if (last)
		_lastPartProjection = &projection;
      else {
		if (_parts.empty()) {
		  // We reserve space to ensure that no reallocation will
		  // happen. Each part has at least two variables, and the
		  // last part is not stored.
		  _parts.reserve(_partialTerm.getVarCount() / 2 - 1);
		}
		_parts.resize(_parts.size() + 1);
		_parts.back().projection = &projection;
		_parts.back().decom.clearAndSetVarCount(projection.getRangeVarCount());
      }

      return true;
    }

    bool doneWithPart() {
      if (_lastPartProjection != 0)
		return true;

      bool hasDecom = (_parts.back().decom.getGeneratorCount() != 0);
      return hasDecom;
    }

  private:
    void generateDecom() {
      if (_parts.empty())
		outputDecom();
      else
		generateDecom(_parts.size() - 1);
    }

    void generateDecom(size_t part) {
      ASSERT(part < _parts.size());
      const Part& p = _parts[part];

      Ideal::const_iterator stop = p.decom.end();
      for (Ideal::const_iterator it = p.decom.begin(); it != stop; ++it) {
		p.projection->inverseProject(_partialTerm, *it);
		if (part == 0)
		  outputDecom();
		else
		  generateDecom(part - 1);
      }
    }

    void outputDecom() {
      ASSERT(_consumer != 0);
      if (_mixedProjectionSubtract == 0 ||
		  !_mixedProjectionSubtract->contains(_partialTerm))
		_consumer->consume(_partialTerm);
    }

    struct Part {
      const Projection* projection;
      Ideal decom;
    };
    vector<Part> _parts;

    Term _partialTerm;
    Ideal* _mixedProjectionSubtract;

    TermConsumer* _consumer;
    const Projection* _lastPartProjection;
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

  SplitType getSplitType(const MsmSlice& slice) {
    return LabelSplit;
  }

  size_t getLabelSplitVariable(const MsmSlice& slice) {
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

  SplitType getSplitType(const MsmSlice& slice) {
    return PivotSplit;
  }

  void getPivot(Term& pivot, MsmSlice& slice) {
	SliceStrategy::getPivot(pivot, slice, _pivotStrategy);
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

  SliceStrategy::PivotStrategy pivotStrategy =
	SliceStrategy::getPivotStrategy(name);

  if (pivotStrategy == SliceStrategy::Unknown) {
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
							 const MsmSlice& slice,
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
    static mpz_class newDegree;
    getDegree(term, _outerPartProjection, newDegree);

    if (newDegree > _partValue) {
      _partProjection->inverseProject(_bound, term);
      _partValue = newDegree;
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
    if (!_improved)
      return;

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
		static mpz_class tmp;
		getDegree(_bound, _projection, tmp);
		_partValue = _toBeat - (tmp - _partValue);
	  }
	} else
	  _partValue = -1;
  }

  void getPivot(Term& pivot, MsmSlice& slice) {
    const Term& lcm = slice.getLcm();

	static mpz_class maxDiff;
	static mpz_class diff;

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

  bool doneWithPart() {
    return _improved;
  }

  const Projection& getCurrentPartProjection() {
    return _outerPartProjection;
  }

  void simplify(MsmSlice& slice) {
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
	  static mpz_class degree;
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
  // The idea here is to see if any inner slices can be discarded,
  // allowing us to move to the outer slice. This has not turned out
  // to work well.
  bool colonSimplify(MsmSlice& slice) {
	bool simplified = true;

	for (size_t var = 0; var < slice.getVarCount(); ++var) {
	  if (slice.getLcm()[var] == 1)
		continue;

	  MsmSlice inner(slice);
	  Term pivot(slice.getVarCount());
	  pivot[var] = slice.getLcm()[var] - 1;
	  inner.innerSlice(pivot);

	  Term bound(inner.getVarCount());
	  Term oldBound(inner.getVarCount());
	  Term colon(inner.getVarCount());
	  getUpperBound(inner, bound);
	  
	  // Obtain bound for degree
	  static mpz_class degree;
	  getDegree(bound, _outerPartProjection, degree);
	  
	  // Check if improvement is possible
	  if (degree <= _partValue) {
		slice.outerSlice(pivot);
		simplified = true;
	  }
	}
	return simplified;
  }

  // For each variable var, the idea is to compute a bound that would
  // apply to the outer slice on the pivot var. If that can be
  // discarded, then we can move to the inner slice. This has not
  // turned out to work well.
  bool involvedBoundSimplify(MsmSlice& slice) {
    if (slice.getIdeal().getGeneratorCount() == 0)
      return false;

    mpz_class degree;

    Term colon(slice.getVarCount());
    Term lcm(slice.getVarCount());
    for (size_t var = 0; var < slice.getVarCount(); ++var) {
      lcm.setToIdentity();

      const Ideal& ideal = slice.getIdeal();
      for (Ideal::const_iterator it = ideal.begin(); it != ideal.end(); ++it)
		if ((*it)[var] <= 1)
		  lcm.lcm(lcm, *it);

      adjustToBound(slice, lcm);
      getDegree(lcm, _outerPartProjection, degree);
      if (degree <= _partValue)
		colon[var] = 1;
    }

    if (colon.isIdentity())
      return false;

    slice.innerSlice(colon);
    return true;
  }

  Exponent improveLowerBound(size_t var,
							 const mpz_class& upperBoundDegree,
							 const Term& upperBound,
							 const Term& lowerBound) {
	if (upperBound[var] == lowerBound[var])
	  return 0;

	size_t outerVar = _outerPartProjection.inverseProjectVar(var);
	static mpz_class baseUpperBoundDegree;
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

	  static mpz_class degreeLess;
	  degreeLess = baseUpperBoundDegree +
		_grader.getGrade(outerVar, lowerBound[var] + mid);

	  if (degreeLess <= _partValue)
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

  void getBasicUpperBound(const MsmSlice& slice, Term& bound) {
    ASSERT(bound.getVarCount() == slice.getVarCount());
  }

  void getUpperBound(const MsmSlice& slice, Term& bound) {
    ASSERT(bound.getVarCount() == slice.getVarCount());
    bound = slice.getLcm();
    adjustToBound(slice, bound);
  }

  void adjustToBound(const MsmSlice& slice, Term& bound) {
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

  virtual void initialize(const MsmSlice& slice) {
    // The purpose of this is to initialize the bound so that it can
    // be used right away, instead of waiting for the slice algorithm
    // to produce some output first.
	if (_useBound) {
	  Term msm;
	  if (computeSingleMSM(slice, msm))
		consume(msm);
	}
  }

  virtual void simplify(MsmSlice& slice) {
    getCurrentSplit()->simplify(slice);
  }

  virtual void consume(const Term& term) {
    getCurrentSplit()->consume(term);
  }

  virtual SplitType getSplitType(const MsmSlice& slice) {
	if (_strategy == 0)
	  return PivotSplit;
	else
	  return _strategy->getSplitType(slice);
  }

  virtual void getPivot(Term& pivot, MsmSlice& slice) {
	if (_strategy != 0)
	  _strategy->getPivot(pivot, slice);
	else
	  getCurrentSplit()->getPivot(pivot, slice);
  }

  virtual size_t getLabelSplitVariable(const MsmSlice& slice) {
	ASSERT(_strategy != 0);
	return _strategy->getLabelSplitVariable(slice);
  }

  virtual void doingIndependenceSplit(const MsmSlice& slice,
									  Ideal* mixedProjectionSubtract) {
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

  virtual bool doneWithIndependentPart() {
    return getCurrentSplit()->doneWithPart();
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
