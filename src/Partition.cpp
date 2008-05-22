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
#include "Partition.h"

#include "Term.h"

Partition::Partition():
  _partitions(0),
  _size(0),
  _capacity(0) {
}

Partition::Partition(const Partition& partition):
  _size(partition._size),
  _capacity(partition._size) {
  _partitions = new int[_size];
  std::copy(partition._partitions,
			partition._partitions + _size, _partitions);
}

Partition::~Partition() {
  delete[] _partitions;
}

void Partition::reset(size_t size) {
  if (size > _capacity) {
    delete[] _partitions;
    _partitions = new int[size];
    _capacity = size;
  }

  _size = size;
  fill_n(_partitions, _size, -1);
}

void Partition::join(size_t i, size_t j) {
  size_t rootI = getRoot(i);
  size_t rootJ = getRoot(j);
  
  if (rootI == rootJ)
    return;
  
  _partitions[rootI] += _partitions[rootJ];
  _partitions[rootJ] = rootI;
}

size_t Partition::getSetCount(size_t minSize) const {
  size_t partitionCount = 0;
  for (size_t i = 0; i < _size; ++i)
    if (i == getRoot(i) &&
	(size_t)-_partitions[i] >= minSize)
      ++partitionCount;
  return partitionCount;
}

size_t Partition::getSizeOfClassOf(size_t i) const {
  return -_partitions[getRoot(i)];
}

size_t Partition::getSetSize(size_t set) const {
  for (size_t i = 0; i < _size; ++i) {
    if (i == getRoot(i)) {
      if (set == 0)
	return -_partitions[i];
      --set;
    }
  }
  ASSERT(false);
  return 0;
}

size_t Partition::getRoot(size_t i) const {
  ASSERT(i < _size);
  if (_partitions[i] >= 0) {
    _partitions[i] = getRoot(_partitions[i]);
    return _partitions[i];
  } else
    return i;
}

size_t Partition::getSize() const {
  return _size;
}

void Partition::print(FILE* file) const {
  fprintf(file, "Partition(size=%lu sets:", (unsigned long)_size);
  for (size_t i = 0; i < _size; ++i)
    fprintf(file, " %li", (long)_partitions[i]);
  fputc('\n', file);
}
