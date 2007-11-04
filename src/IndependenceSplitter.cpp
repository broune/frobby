#include "stdinc.h"
#include "IndependenceSplitter.h"

#include "Slice.h"
#include "Ideal.h"
#include "Term.h"

IndependenceSplitter::IndependenceSplitter(const Partition& partition,
					   Slice& slice):
  _slice(slice) {
  _childCount = partition.getSetCount();

  for (size_t child = 0; child < _childCount; ++child) {
    size_t childVarCount = partition.getSetSize(child);

    if (childVarCount == 1) {
      _anySingletons = true;
      continue;
    }

    _children.resize(_children.size() + 1);

    _children.back().projection.reset(partition, child);
    _children.back().ideal = _slice.getIdeal()->createNew(childVarCount);
  }
  _childCount = _children.size();
}

void IndependenceSplitter::computePartition
(Partition& partition, const Slice& slice) {
  partition.reset(slice.getVarCount());

  Ideal::const_iterator idealEnd = slice.getIdeal()->end();
  for (Ideal::const_iterator it = slice.getIdeal()->begin();
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
      slice.getIdeal()->getGeneratorCount() < 15)
    return false;
  else
    return true;
}

size_t IndependenceSplitter::getChildCount() const {
  return _childCount;
}

void IndependenceSplitter::setCurrentChild(size_t child,
					   Slice& projSlice) {
  ASSERT(child < _childCount);

  _currentChild = child;

  Projection& projection = _children[child].projection;
  size_t varCount = projection.getRangeVarCount();

  projSlice.reset(_slice.getIdeal()->createNew(varCount),
		  _slice.getSubtract()->createNew(varCount),
		  varCount);

  projection.project(projSlice.getMultiply(), _slice.getMultiply());

  Term tmp(varCount);
  Ideal::const_iterator idealEnd = _slice.getIdeal()->end();
  for (Ideal::const_iterator it = _slice.getIdeal()->begin();
       it != idealEnd; ++it) {
    projection.project(tmp, *it);
    if (!tmp.isIdentity())
      projSlice.getIdeal()->insert(tmp);
  }
}

void IndependenceSplitter::consume(const Term& term) {
  _children[_currentChild].ideal->insert(term);
}

void IndependenceSplitter::generateDecom(DecomConsumer* consumer) {
  Term partial(_slice.getVarCount());

  // This relies on _slice.getIdeal() to be minimized.
  if (_anySingletons > 0) {
    partial = _slice.getLcm();
    for (size_t i = 0; i < _slice.getVarCount(); ++i)
      if (partial[i] > 0)
	partial[i] = _slice.getMultiply()[i] + (partial[i] - 1);
  }

  generateDecom(consumer, 0, partial);
}

void IndependenceSplitter::generateDecom(DecomConsumer* consumer,
					 size_t child,
					 Term& partial) {
  if (child == _childCount) {
    consumer->consume(partial);
    return;
  }

  const Child& c = _children[child];
  Ideal::const_iterator stop = c.ideal->end();
  for (Ideal::const_iterator it = c.ideal->begin(); it != stop; ++it) {
    c.projection.inverseProject(partial, *it);
    generateDecom(consumer, child + 1, partial);
  }
}
