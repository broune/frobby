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

IndependenceSplitter::IndependenceSplitter(const Partition& partition,
										   MsmSlice* slice) {
  ASSERT(partition.getSetCount() == 2);

  _leftProjection.reset(partition, 0);
  _leftSlice.setToProjOf(*slice, _leftProjection);

  _rightProjection.reset(partition, 1);
  _rightSlice.setToProjOf(*slice, _rightProjection);
}

bool IndependenceSplitter::computePartition
(Partition& partition, const MsmSlice* slice) {
  partition.reset(slice->getVarCount());

  Ideal::const_iterator stop = slice->getIdeal().end();
  for (Ideal::const_iterator it = slice->getIdeal().begin();
       it != stop; ++it) {
    size_t first = ::getFirstNonZeroExponent(*it, slice->getVarCount());
    for (size_t var = first + 1; var < slice->getVarCount(); ++var)
      if ((*it)[var] > 0)
	partition.join(first, var);
  }

  // TODO: eliminate code duplication from above.
  stop = slice->getSubtract().end();
  for (Ideal::const_iterator it = slice->getSubtract().begin();
       it != stop; ++it) {
    size_t first = ::getFirstNonZeroExponent(*it, slice->getVarCount());
    for (size_t var = first + 1; var < slice->getVarCount(); ++var)
      if ((*it)[var] > 0)
	partition.join(first, var);
  }

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

  if (at2 == 0 && over2 <= 1)
	return false;

  if (partition.getSetCount() > 2) {
	size_t maxSet = 0;
	for (size_t set = 1; set < partition.getSize(); ++set)
	  if (partition.getSizeOfClassOf(set) >
		  partition.getSizeOfClassOf(maxSet))
		maxSet = partition.getRoot(set);

	size_t nonMaxSet = 0;
	for (size_t set = 0; set < partition.getSize(); ++set)
	  if (partition.getRoot(maxSet) != partition.getRoot(set))
		nonMaxSet = set;
	ASSERT(partition.getRoot(maxSet) != partition.getRoot(nonMaxSet));

	for (size_t set = 0; set < partition.getSize(); ++set)
	  if (partition.getRoot(set) != maxSet)
		partition.join(set, nonMaxSet);
  }
  ASSERT(partition.getSetCount() == 2);

  return true;
}
