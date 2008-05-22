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

#include "Ideal.h"
#include "Term.h"
#include "Projection.h"

void IndependenceSplitter::getBigProjection(Projection& projection) const {
  projection.reset(_partition, _bigSet);
}

void IndependenceSplitter::getRestProjection(Projection& projection) const {
  projection.reset(_partition, 1 - _bigSet);
}

bool IndependenceSplitter::analyze(const Slice& slice) {
  _partition.reset(slice.getVarCount());
  
  Ideal::const_iterator stop = slice.getIdeal().end();
  for (Ideal::const_iterator it = slice.getIdeal().begin();
       it != stop; ++it) {
    size_t first = ::getFirstNonZeroExponent(*it, slice.getVarCount());
    for (size_t var = first + 1; var < slice.getVarCount(); ++var)
      if ((*it)[var] > 0)
		_partition.join(first, var);
  }

  // TODO: eliminate code duplication from above.
  stop = slice.getSubtract().end();
  for (Ideal::const_iterator it = slice.getSubtract().begin();
       it != stop; ++it) {
    size_t first = ::getFirstNonZeroExponent(*it, slice.getVarCount());
    for (size_t var = first + 1; var < slice.getVarCount(); ++var)
      if ((*it)[var] > 0)
		_partition.join(first, var);
  }

  size_t childCount = _partition.getSetCount();

  if (childCount == 1)
    return false;

  _oneVarCount = 0;
  _twoVarCount = 0;
  _moreThanTwoVarCount = 0;
  for (size_t i = 0; i < childCount; ++i) {
    size_t size = _partition.getSetSize(i);
    if (size == 1)
      ++_oneVarCount;
    else if (size == 2)
      ++_twoVarCount;
    else
      ++_moreThanTwoVarCount;
  }

  if (_twoVarCount == 0 && _moreThanTwoVarCount <= 1)
	return false;

  if (_partition.getSetCount() > 2) {
	size_t maxSet = 0;
	for (size_t set = 1; set < _partition.getSize(); ++set)
	  if (_partition.getSizeOfClassOf(set) >
		  _partition.getSizeOfClassOf(maxSet))
		maxSet = _partition.getRoot(set);

	size_t nonMaxSet = 0;
	for (size_t set = 0; set < _partition.getSize(); ++set)
	  if (_partition.getRoot(maxSet) != _partition.getRoot(set))
		nonMaxSet = set;
	ASSERT(_partition.getRoot(maxSet) != _partition.getRoot(nonMaxSet));

	for (size_t set = 0; set < _partition.getSize(); ++set)
	  if (_partition.getRoot(set) != maxSet)
		_partition.join(set, nonMaxSet);
  }
  ASSERT(_partition.getSetCount() == 2);

  _bigSet = 0; // TODO: make this actually correct.

  return true;
}
