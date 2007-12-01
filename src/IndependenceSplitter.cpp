#include "stdinc.h"
#include "IndependenceSplitter.h"

#include "Slice.h"
#include "Ideal.h"
#include "Term.h"

IndependenceSplitter::IndependenceSplitter(const Partition& partition,
					   Slice& slice):
  _slice(slice),
  _childCount(0),
  _singletonDecom(slice.getVarCount()),
  _mixedProjectionSubtract(0) {

  initializeChildren(partition);

  // A term belongs to the child childAt[i] if term[i] > 0.
  vector<Child*> childAt(slice.getVarCount()); 
  for (size_t c = 0; c < _children.size(); ++c)
    for (size_t var = 0; var < _children[c].projection.getRangeVarCount();
	 ++var)
      childAt[_children[c].projection.getDomainVar(var)] = &(_children[c]);

  populateChildIdealsAndSingletonDecom(childAt);
  populateChildSubtracts(childAt);

  _childCount = _children.size();

  _slice.clear(); // to save memory

  // Handle the smallest children first to save memory.
  //sort(_children.begin(), _children.end());
  // commented out since it is not so hot to copy around the Ideals.
}

// Set up entries in _children for those partition sets that are not
// singletons (i.e. contains more than one variable).
void IndependenceSplitter::initializeChildren(const Partition& partition) {
  size_t setCount = partition.getSetCount();
  for (size_t set = 0; set < setCount; ++set) {
    size_t childVarCount = partition.getSetSize(set);

    if (childVarCount == 1)
      continue;

    _children.resize(_children.size() + 1);
    Child& child = _children.back();

    child.slice.resetAndSetVarCount(childVarCount);
    child.projection.reset(partition, set);
    child.decom.clearAndSetVarCount(childVarCount);
  }
}

// Distribute the the terms of _slice.getIdeal() to the ideals of the
// children. Also set up _singletonDecom to be the lcm of the
// decompositions of the singleton children (i.e. those that contain
// only one variable). Each of these decompositions contain only 1
// element, so this is easy to do and _singletonDecom ends up
// containing all the information we need about the singleton
// decompositions.
void IndependenceSplitter::populateChildIdealsAndSingletonDecom
(const vector<Child*>& childAt) {
  // tmp is static to avoid an allocation for each call.
  static Term tmp;
  if (tmp.getVarCount() < _slice.getVarCount())
    tmp.reset(_slice.getVarCount());
  Exponent* exponents = tmp.begin();

  Ideal::const_iterator idealEnd = _slice.getIdeal().end();
  for (Ideal::const_iterator it = _slice.getIdeal().begin();
       it != idealEnd; ++it) {
    size_t var = getFirstNonZeroExponent(*it, _slice.getVarCount());
    ASSERT(var < _slice.getVarCount());

    Child* child = childAt[var];
    if (child == 0) {
      // This relies on _slice.getIdeal() being minimized.
      _singletonDecom[var] = _slice.getMultiply()[var] + ((*it)[var] - 1);
      continue;
    }

    child->projection.project(exponents, *it);
    ASSERT(!isIdentity(exponents, child->projection.getRangeVarCount()));

    child->slice.insertIntoIdeal(exponents);
  }
}

// Distributes _slice.getSubtract() among the children. Terms that
// project into more than one child are saved in
// _mixedProjectionSubtract.
void IndependenceSplitter::populateChildSubtracts
(const vector<Child*>& childAt) {
  // tmp is static to avoid an allocation for each call.
  static Term tmp;
  if (tmp.getVarCount() < _slice.getVarCount())
    tmp.reset(_slice.getVarCount());
  Exponent* exponents = tmp.begin();

  Ideal::const_iterator subtractEnd = _slice.getSubtract().end();
  for (Ideal::const_iterator it = _slice.getSubtract().begin();
       it != subtractEnd; ++it) {
    bool hasMixedProjection = false;

    Child* child = 0;
    bool seenNonZeroExponent = false;
    for (size_t var = 0; var < _slice.getVarCount(); ++var) {
      if ((*it)[var] == 0)
	continue;
      if (!seenNonZeroExponent) {
	child = childAt[var];
	seenNonZeroExponent = true;
      } else if (child != childAt[var]) {
	hasMixedProjection = true;
	break;
      }
    }

    if (hasMixedProjection) {
      if (_mixedProjectionSubtract == 0)
	_mixedProjectionSubtract = new Ideal(_slice.getVarCount());
      
      product(exponents, *it, _slice.getMultiply(), _slice.getVarCount());
      _mixedProjectionSubtract->insert(exponents);
    } else if (child != 0) {
      // child == 0 implies that *it projects onto a singleton.
      child->projection.project(exponents, *it);
      ASSERT(!isIdentity(exponents, child->projection.getRangeVarCount()));

      child->slice.getSubtract().insert(exponents);
    }
  }
}

IndependenceSplitter::~IndependenceSplitter() {
}

void IndependenceSplitter::computePartition
(Partition& partition, const Slice& slice) {
  partition.reset(slice.getVarCount());

  Ideal::const_iterator idealEnd = slice.getIdeal().end();
  for (Ideal::const_iterator it = slice.getIdeal().begin();
       it != idealEnd; ++it) {
    size_t first = ::getFirstNonZeroExponent(*it, slice.getVarCount());
    for (size_t var = first + 1; var < slice.getVarCount(); ++var)
      if ((*it)[var] > 0)
	partition.join(first, var);
  }
}

bool IndependenceSplitter::shouldPerformSplit
(const Partition& partition, const Slice& slice) {
  size_t childCount = partition.getSetCount();

  if (childCount == 1)
    return false;

  size_t at1 = 0;
  size_t at2 = 0;
  size_t over2 = 0;
  for (size_t i = 0; i < childCount; ++i) {
    size_t size = partition.getSetSize(i);
    if (size == 1)
      ++at1;
    else if (size == 2)
      ++at2;
    else
      ++over2;
  }
  
  if (at1 <= 2 && at2 == 0 && over2 <= 1 &&
      slice.getIdeal().getGeneratorCount() < 15)
    return false;
  else
    return true;
}

size_t IndependenceSplitter::getChildCount() const {
  return _childCount;
}

void IndependenceSplitter::setCurrentChild(size_t childIndex,
					   Slice& projSlice) {
  ASSERT(childIndex < _childCount);

  _currentChild = childIndex;
  Child& child = _children[childIndex];

  projSlice.clear();
  projSlice.swap(child.slice);

  child.projection.project(projSlice.getMultiply(), _slice.getMultiply());
}

bool IndependenceSplitter::currentChildDecomIsEmpty() const {
  ASSERT(_currentChild < _children.size());

  return _children[_currentChild].decom.getGeneratorCount() == 0;
}

void IndependenceSplitter::consume(const Term& term) {
  _children[_currentChild].decom.insert(term);
}

void IndependenceSplitter::generateDecom(DecomConsumer* consumer) {
  generateDecom(consumer, 0, _singletonDecom);
}

// This method tries out all possible ways to combine one
// decomposition element from each child. The singleton children are
// already dealth with because we start out with partial being equal
// to _singletonDecom. We also need to discard the decomposition
// elements that we can make this way that are contained within
// _mixedProjectionSubtract.
void IndependenceSplitter::generateDecom(DecomConsumer* consumer,
					 size_t child,
					 Term& partial) {
  if (child == _childCount) {
    if (_mixedProjectionSubtract == 0 ||
	!_mixedProjectionSubtract->contains(partial))
      consumer->consume(partial);
    return;
  }

  const Child& c = _children[child];
  Ideal::const_iterator stop = c.decom.end();
  for (Ideal::const_iterator it = c.decom.begin(); it != stop; ++it) {
    c.projection.inverseProject(partial, *it);
    generateDecom(consumer, child + 1, partial);
  }
}

bool IndependenceSplitter::Child::operator<(const Child& child) const {
  if (projection.getRangeVarCount() != child.projection.getRangeVarCount())
    return projection.getRangeVarCount() < child.projection.getRangeVarCount();
  else
    return slice.getIdeal().getGeneratorCount() <
      child.slice.getIdeal().getGeneratorCount();
}
