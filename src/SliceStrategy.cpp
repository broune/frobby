#include "stdinc.h"
#include "SliceStrategy.h"

#include "Slice.h"
#include "Term.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include <vector>
#include "Projection.h"
#include "SliceAlgorithm.h"
#include "TermGrader.h"

SliceStrategy::~SliceStrategy() {
}

void SliceStrategy::initialize(const Slice& slice) {
}

void SliceStrategy::startingContent(const Slice& slice) {
}

void SliceStrategy::endingContent() {
}

void SliceStrategy::simplify(Slice& slice) {
  slice.simplify();
}

void SliceStrategy::getPivot(Term& pivot, Slice& slice) {
  fputs("INTERNAL ERROR: Undefined SliceStrategy::getPivot called.\n", stderr);
  ASSERT(false);
  exit(1);
}

size_t SliceStrategy::getLabelSplitVariable(const Slice& slice) {
  fputs
    ("INTERNAL ERROR: Undefined SliceStrategy::getLabelSplitVariable called.\n",
     stderr);
  ASSERT(false);
  exit(1);
}

class DecomSliceStrategy : public SliceStrategy {
public:
  DecomSliceStrategy(TermConsumer* consumer):
    _consumer(consumer) {
  }

  virtual ~DecomSliceStrategy() {
    ASSERT(_independenceSplits.empty());
    delete _consumer;
  }

  virtual void consume(const Term& term) {
    ASSERT(getCurrentConsumer() != 0);

    getCurrentConsumer()->consume(term);
  }

  virtual void doingIndependenceSplit(const Slice& slice,
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
    IndependenceSplit(const Slice& slice,
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

class LabelSliceStrategy : public DecomSliceStrategy {
public:
  enum Type {
	MaxLabel,
	MinLabel,
    VarLabel
  };

  LabelSliceStrategy(Type type, TermConsumer* consumer):
    DecomSliceStrategy(consumer),
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

class PivotSliceStrategy : public DecomSliceStrategy {
public:
  enum Type {
    Minimum,
    Median,
    Maximum,
	MinGen,
	Indep,
	GCD
  };

  PivotSliceStrategy(Type type, TermConsumer* consumer):
    DecomSliceStrategy(consumer),
    _type(type) {
	srand(0); // to make things repeatable
  }
 
  SplitType getSplitType(const Slice& slice) {
    return PivotSplit;
  }

  void getPivot(Term& pivot, Slice& slice) {
	ASSERT(pivot.getVarCount() == slice.getVarCount());

	if (_type == MinGen) {
	  getMinGenPivot(pivot, slice);
	  return;
	}

	if (_type == Indep) {
	  if (getIndependencePivot(slice, pivot))
		return;
	}

	size_t var = getBestVar(slice);
	switch (_type) {
	case Minimum:
	  pivot.setToIdentity();
	  pivot[var] = 1;
	  break;

	case Maximum:
	  pivot.setToIdentity();
	  pivot[var] = slice.getLcm()[var] - 1;
	  break;

	case Indep: // Indep uses MidPure as a fall-back.
	case Median:
	  pivot.setToIdentity();
	  pivot[var] = getMedianPositiveExponentOf(slice, var);
	  if (pivot[var] == slice.getLcm()[var])
		pivot[var] -= 1;
	  break;

	case GCD:
	  getGCDPivot(slice, pivot, var);
	  break;

	default:
	  fputs("INTERNAL ERROR: Undefined pivot split type.\n", stderr);
	  ASSERT(false);
	  exit(1);
    }
  }

private:
  size_t getRandomSupportVar(const Term& term) {
	ASSERT(!term.isIdentity());

	size_t selected = rand() % term.getSizeOfSupport();
	for (size_t var = 0; ; ++var) {
	  ASSERT(var < term.getVarCount());
	  if (term[var] == 0)
		continue;

	  if (selected == 0)
		return var;
	  --selected;
	}
  }

  void getGCDPivot(Slice& slice, Term& pivot, size_t var) {
	size_t nonDivisibleCount = 0;
	Ideal::const_iterator end = slice.getIdeal().end();
	for (Ideal::const_iterator it = slice.getIdeal().begin();
		 it != end; ++it)
	  if ((*it)[var] >= 2)
		++nonDivisibleCount;
	
	for (int i = 0; i < 3; ++i) {
	  size_t selected = rand() % nonDivisibleCount;
	  for (Ideal::const_iterator it = slice.getIdeal().begin(); ; ++it) {
		ASSERT(it != end);
		if ((*it)[var] < 2)
		  continue;

		if (selected == 0) {
		  if (i == 0)
			pivot = *it;
		  else
			pivot.gcd(pivot, *it);
		  break;
		}
		--selected;
	  }
	}

	pivot.decrement();
  }

  void getMinGenPivot(Term& pivot, Slice& slice) {
	size_t nonSquareFreeCount = 0;
	Ideal::const_iterator end = slice.getIdeal().end();
	for (Ideal::const_iterator it = slice.getIdeal().begin();
		 it != end; ++it)
	  if (!::isSquareFree(*it, slice.getVarCount()))
		++nonSquareFreeCount;

	size_t selected = rand() % nonSquareFreeCount;
	for (Ideal::const_iterator it = slice.getIdeal().begin(); ; ++it) {
	  ASSERT(it != end);
	  if (::isSquareFree(*it, slice.getVarCount()))
		continue;

	  if (selected == 0) {
		pivot = *it;
		break;
	  }
	  --selected;
	}

	pivot.decrement();
  }

  Exponent getMedianPositiveExponentOf(Slice& slice, size_t var) {
	slice.singleDegreeSortIdeal(var);
	Ideal::const_iterator end = slice.getIdeal().end();
	Ideal::const_iterator begin = slice.getIdeal().begin();
	while ((*begin)[var] == 0) {
	  ++begin;
	  ASSERT(begin != end);
	}
	return (*(begin + (distance(begin, end) ) / 2))[var];
  }

  // Returns the variable that divides the most minimal generators of
  // those where some minimal generator is divisible by the square of
  // that variable.
  size_t getBestVar(Slice& slice) {
    Term co(slice.getVarCount());
	slice.getIdeal().getSupportCounts(co);

    const Term& lcm = slice.getLcm();
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  if (lcm[var] <= 1)
		co[var] = 0;

	ASSERT(!co.isIdentity());
	
	Exponent maxCount = co[co.getFirstMaxExponent()];
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  if (co[var] < maxCount)
		co[var] = 0;

	// Choose a deterministically random variable among those that are
	// best. This helps to avoid getting into a bad pattern.
	return getRandomSupportVar(co);
  }

  bool getIndependencePivot(Slice& slice, Term& pivot) {
	if (slice.getVarCount() == 1)
	  return false;

	for (int attempts = 0; attempts < 10; ++attempts) {
	  // Pick two distinct variables.
	  size_t var1 = rand() % slice.getVarCount();
	  size_t var2 = rand() % (slice.getVarCount() - 1);
	  if (var2 >= var1)
		++var2;

	  // Make pivot as big as it can be while making var1 and var2
	  // independent of each other.
	  bool first = true;
	  Ideal::const_iterator end = slice.getIdeal().end();
	  for (Ideal::const_iterator it = slice.getIdeal().begin();
		   it != end; ++it) {
		if ((*it)[var1] == 0 || (*it)[var2] == 0)
		  continue;

		if (first)
		  pivot = *it;
		else {
		  for (size_t var = 0; var < slice.getVarCount(); ++var)
			if (pivot[var] >= (*it)[var])
			  pivot[var] = (*it)[var] - 1;
		}
	  }

	  if (!first && !pivot.isIdentity())
		return true;
	}

	return false;
  }

  Type _type;
};

SliceStrategy* SliceStrategy::newDecomStrategy(const string& name,
											   TermConsumer* consumer) {
  if (name == "maxlabel")
    return new LabelSliceStrategy(LabelSliceStrategy::MaxLabel, consumer);
  else if (name == "minlabel")
    return new LabelSliceStrategy(LabelSliceStrategy::MinLabel, consumer);
  else if (name == "varlabel")
    return new LabelSliceStrategy(LabelSliceStrategy::VarLabel, consumer);
  else if (name == "minimum")
    return new PivotSliceStrategy(PivotSliceStrategy::Minimum, consumer);
  else if (name == "median")
    return new PivotSliceStrategy(PivotSliceStrategy::Median, consumer);
  else if (name == "maximum")
    return new PivotSliceStrategy(PivotSliceStrategy::Maximum, consumer);
  else if (name == "mingen")
    return new PivotSliceStrategy(PivotSliceStrategy::MinGen, consumer);
  else if (name == "indep")
    return new PivotSliceStrategy(PivotSliceStrategy::Indep, consumer);
  else if (name == "gcd")
    return new PivotSliceStrategy(PivotSliceStrategy::GCD, consumer);

  fprintf(stderr, "ERROR: Unknown split strategy \"%s\".\n", name.c_str());
  exit(1);
}

// A decorator (a design pattern) for a SliceStrategy that simply
// delegates all calls. The purpose of this class is to act as a
// convenient base class for other decorators.
class DecoratorSliceStrategy : public SliceStrategy {
public:
  DecoratorSliceStrategy(SliceStrategy* strategy):
    _strategy(strategy) {
    ASSERT(strategy != 0);
  }

  virtual ~DecoratorSliceStrategy() {
    delete _strategy;
  }

  virtual void initialize(const Slice& slice) {
    _strategy->initialize(slice);
  }

  virtual void doingIndependenceSplit(const Slice& slice,
				      Ideal* mixedProjectionSubtract) {
    _strategy->doingIndependenceSplit
      (slice, mixedProjectionSubtract);
  }

  virtual void doingIndependentPart(const Projection& projection, bool last) {
    _strategy->doingIndependentPart(projection, last);
  }

  virtual bool doneWithIndependentPart() {
    return _strategy->doneWithIndependentPart();
  }

  virtual void doneWithIndependenceSplit() {
    _strategy->doneWithIndependenceSplit();
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

  virtual void getPivot(Term& pivot, Slice& slice) {
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

  void getPivot(Term& pivot, Slice& slice) {
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
  bool colonSimplify(Slice& slice) {
	bool simplified = true;

	for (size_t var = 0; var < slice.getVarCount(); ++var) {
	  if (slice.getLcm()[var] == 1)
		continue;

	  Slice inner(slice);
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
  bool involvedBoundSimplify(Slice& slice) {
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

  void getBasicUpperBound(const Slice& slice, Term& bound) {
    ASSERT(bound.getVarCount() == slice.getVarCount());
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
};

// Ignores subtraction of mixed generators when doing independence
// splits. This is no problem right now, but it would become a problem
// if SliceAlgorithm at a later point gets the (externally exposed)
// capability to compute slices in general, rather than a complete
// decomposition.
//
// Also ignores the possibility of "fake" artinians (e.g. in getPivot).
//
// The passed-in strategy is only used as a split strategy, and it
// does NOT get informed about anything other than pivot and label
// split requests. If it is null, then a specialized grade-base pivot
// selection algorithm is used.
class FrobeniusSliceStrategy : public SliceStrategy {
public:
  FrobeniusSliceStrategy(SliceStrategy* strategy,
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

  virtual ~FrobeniusSliceStrategy() {
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

  SliceStrategy* _strategy;
  TermConsumer* _consumer;
  vector<FrobeniusIndependenceSplit*> _independenceSplits;
  const TermGrader& _grader;
  bool _useBound;
};

SliceStrategy* SliceStrategy::
newFrobeniusStrategy(const string& name,
					 TermConsumer* consumer,
					 TermGrader& grader,
					 bool useBound) {
  SliceStrategy* strategy;
  if (name == "frob")
	strategy = 0;
  else
	strategy = newDecomStrategy(name, 0);

  return new FrobeniusSliceStrategy(strategy, consumer, grader, useBound);
}

class StatisticsSliceStrategy : public DecoratorSliceStrategy {
public:
  StatisticsSliceStrategy(SliceStrategy* strategy):
    DecoratorSliceStrategy(strategy),
    _level(0),
    _consumeCount(0),
    _independenceSplitCount(0) {
  }

  ~StatisticsSliceStrategy() {
    fputs("**** Statistics\n", stderr);
    size_t sum = 0;
    size_t baseSum = 0;
	size_t emptySum = 0;
    for (size_t l = 0; l < _calls.size(); ++l) {
      sum += _calls[l];
      baseSum += _baseCases[l];
	  emptySum += _empty[l];
      if (false) {
		fprintf(stderr,
				"Level %lu had %lu calls of which %lu were base cases.\n",
				(unsigned long)l + 1,
				(unsigned long)_calls[l],
				(unsigned long)_baseCases[l]);
      }
    }

    fprintf(stderr,
			"* recursive levels:    %lu\n"
			"* decom size:          %lu\n" 
			"* empty slices:        %lu\n"
			"* base case slices:    %lu\n"
			"* independence splits: %lu\n"
			"* (numbers not corrected for independence splits)\n"
			"****\n",
			(unsigned long)_calls.size(),
			(unsigned long)_consumeCount,
			(unsigned long)emptySum,
			(unsigned long)baseSum,
			(unsigned long)_independenceSplitCount);
  }

  virtual void doingIndependenceSplit(const Slice& slice,
									  Ideal* mixedProjectionSubtract) {
    ++_independenceSplitCount;
    DecoratorSliceStrategy::doingIndependenceSplit(slice,
												   mixedProjectionSubtract);
  }

  void startingContent(const Slice& slice) {
    if (_level > 0)
      _isBaseCase[_level - 1] = false;

    if (_calls.size() == _level) {
      _calls.push_back(0);
      _baseCases.push_back(0);
      _isBaseCase.push_back(true);
	  _empty.push_back(0);
	  _isEmpty.push_back(true);
    }

    _isBaseCase[_level] = true;
	_isEmpty[_level] = true;
    ++_calls[_level];
    ++_level;

    DecoratorSliceStrategy::startingContent(slice);
  }

  void endingContent() {
    --_level;
    if (_isBaseCase[_level])
      ++_baseCases[_level];
	if (_isEmpty[_level])
	  ++_empty[_level];

    DecoratorSliceStrategy::endingContent();
  }

  void consume(const Term& term) {
	for (size_t level = _level; level > 0; --level) {
	  if (!_isEmpty[level - 1])
		break; // The remaining entries are already false
	  _isEmpty[level - 1] = false;
	}
    ++_consumeCount;

    DecoratorSliceStrategy::consume(term);
  }

private:
  size_t _level;
  vector<size_t> _calls;
  vector<size_t> _baseCases;
  vector<size_t> _isBaseCase;
  vector<size_t> _empty;
  vector<size_t> _isEmpty;
  size_t _consumeCount;
  size_t _independenceSplitCount;
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
  
  virtual void doingIndependenceSplit(const Slice& slice,
				      Ideal* mixedProjectionSubtract) {
    fprintf(stderr, "DEBUG %lu: doing independence split.\n",
	    (unsigned long)_level);
    fflush(stderr);
    DecoratorSliceStrategy::doingIndependenceSplit
      (slice, mixedProjectionSubtract);
  }

  virtual void doingIndependentPart(const Projection& projection, bool last) {
    fprintf(stderr, "DEBUG %lu: doing independent part\n",
	    (unsigned long)_level);
    if (last)
      fputs(" (last)", stderr);
    fputs(".\n", stderr);
    fflush(stderr);
    DecoratorSliceStrategy::doingIndependentPart(projection, last);
  }

  virtual bool doneWithIndependentPart() {
    fprintf(stderr, "DEBUG %lu: done with that independent part.\n",
	    (unsigned long)_level);
    fflush(stderr);
    
    return DecoratorSliceStrategy::doneWithIndependentPart();
  }
  
  virtual void doneWithIndependenceSplit() {
    fprintf(stderr, "DEBUG %lu: done with independence split.\n",
	    (unsigned long)_level);
    fflush(stderr);
    DecoratorSliceStrategy::doneWithIndependenceSplit();
  }

  void startingContent(const Slice& slice) {
    ++_level;
    fprintf(stderr, "DEBUG %lu: computing content of the following slice.\n",
	    (unsigned long)_level);
    slice.print(stderr);
    fflush(stderr);

    DecoratorSliceStrategy::startingContent(slice);
  }

  void endingContent() {
    fprintf(stderr, "DEBUG %lu: done computing content of that slice.\n",
	    (unsigned long)_level);
    fflush(stderr);
    --_level;

    DecoratorSliceStrategy::endingContent();
  }

  void getPivot(Term& pivot, Slice& slice) {
    DecoratorSliceStrategy::getPivot(pivot, slice);
    fprintf(stderr, "DEBUG %lu: performing pivot split on ",
	    (unsigned long)_level);
    pivot.print(stderr);
    fputs(".\n", stderr);
    fflush(stderr);
  }

  size_t getLabelSplitVariable(const Slice& slice) {
    size_t var = DecoratorSliceStrategy::getLabelSplitVariable(slice);
    fprintf(stderr, "DEBUG %lu: performing label split on var %lu.\n",
	    (unsigned long)_level,
	    (unsigned long)var);
    fflush(stderr);
    return var;
  }

  void consume(const Term& term) {
    fprintf(stderr, "DEBUG %lu: Writing ", (unsigned long)_level);
    term.print(stderr);
    fputs(" to output.\n", stderr);
    fflush(stderr);

    DecoratorSliceStrategy::consume(term);
  }
  
private:
  size_t _level;
};

SliceStrategy* SliceStrategy::addDebugOutput(SliceStrategy* strategy) {
  return new DebugSliceStrategy(strategy);
}
