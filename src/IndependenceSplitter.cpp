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
#include "IndependenceSplitter.h"

#include "Slice.h"
#include "Ideal.h"
#include "Term.h"

IndependenceSplitter::IndependenceSplitter(const Partition& partition,
					   Slice& slice):
  _slice(slice),
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

  for (size_t i = 0; i < _children.size(); ++i) {
    _children[i].projection.project
      (_children[i].slice.getMultiply(), _slice.getMultiply());
  }

  // Handle the smallest children first to save memory.
  //
  // A simple sort. The STL sort is not used to ensure that an
  // efficient swap method is utilized.
  for (size_t i = 0; i < _children.size(); ++i) {
    for (size_t j = 1; j < _children.size(); ++j) {
      Child& a = _children[j - 1];
      Child& b = _children[j];
      if (b < a)
	a.swap(b);
    }
  }
}

bool IndependenceSplitter::Child::operator<(const Child& child) const {
  if (slice.getVarCount() == child.slice.getVarCount())
    return slice.getIdeal().getGeneratorCount() <
      child.slice.getIdeal().getGeneratorCount();
  else
    return slice.getVarCount() < child.slice.getVarCount();
}

void IndependenceSplitter::Child::swap(Child& child) {
  slice.swap(child.slice);
  projection.swap(child.projection);
}

IndependenceSplitter::~IndependenceSplitter() {
  delete _mixedProjectionSubtract;
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
    if (child == 0)
      continue;

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

  return at2 > 0 || over2 > 1;
}

Ideal* IndependenceSplitter::getMixedProjectionSubtract() {
  return _mixedProjectionSubtract;
}

size_t IndependenceSplitter::getChildCount() const {
  return _children.size();
}


Slice& IndependenceSplitter::getSlice(size_t part) {
  ASSERT(part < _children.size());
  return _children[part].slice;
}

Projection& IndependenceSplitter::getProjection(size_t part) {
  ASSERT(part < _children.size());
  return _children[part].projection;
}
