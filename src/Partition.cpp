#include "stdinc.h"
#include "Partition.h"

#include "Term.h"

Partition::Partition():
  _partitions(0),
  _size(0),
  _capacity(0) {
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

void Partition::print(ostream& out) const {
  out << "Partition(size=" << _size << " sets:";
  for (size_t i = 0; i < _size; ++i)
    out << ' ' << _partitions[i];
  out << endl;
}

size_t Partition::getSize() const {
  return _size;
}
