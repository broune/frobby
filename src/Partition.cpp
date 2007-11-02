#include "stdinc.h"
#include "Partition.h"

#include "Term.h"

Partition::Partition(int size):
  _partitions(new int[size]),
  _size(size) {
  fill_n(_partitions, _size, -1);
}

Partition::~Partition() {
  delete[] _partitions;
}

void Partition::join(int i, int j) {
  int rootI = getRoot(i);
  int rootJ = getRoot(j);
  
  if (rootI == rootJ)
    return;
  
  _partitions[rootI] += _partitions[rootJ];
  _partitions[rootJ] = rootI;
}

int Partition::getSetCount(int minSize) const {
  int partitionCount = 0;
  for (int i = 0; i < _size; ++i)
    if (i == getRoot(i) &&
	-_partitions[i] >= minSize)
      ++partitionCount;
  return partitionCount;
}

int Partition::getSetSize(int set) const {
  for (int i = 0; i < _size; ++i) {
    if (i == getRoot(i)) {
      if (set == 0)
	return -_partitions[i];
      --set;
    }
  }
  ASSERT(false);
  return 0;
}

int Partition::getRoot(int i) const {
  ASSERT(i < _size);
  if (_partitions[i] >= 0) {
    _partitions[i] = getRoot(_partitions[i]);
    return _partitions[i];
  } else
    return i;
}

void Partition::getProjection(int number,
			      vector<Exponent>& projection) const {
  
  projection.resize(getSetSize(number));

  int root = -1;
  for (int i = 0; i < _size; ++i) {
    if (i == getRoot(i)) {
      if (number == 0) {
	root = i;
	break;
      }
      --number;
    }
  }
  ASSERT(number == 0 && root != -1);

  size_t projectionOffset = 0;

  for (size_t i = 0; i < (size_t)_size; ++i) {
    if (getRoot(i) != root)
      continue;

    projection[projectionOffset] = i;
    ++projectionOffset;
  }
}

void Partition::print(ostream& out) const {
  out << "Partition(size=" << _size << " sets:";
  for (int i = 0; i < _size; ++i)
    out << ' ' << _partitions[i];
  out << endl;
}

void Partition::project(Term& to, const Exponent* from,
	     const vector<Exponent>& projection) const {
  size_t size = projection.size();
  for (size_t i = 0; i < size; ++i)
    to[i] = from[projection[i]];
}

void Partition::inverseProject(Term& to, const Exponent* from,
	     const vector<Exponent>& projection) const {
  size_t size = projection.size();
  for (size_t i = 0; i < size; ++i)
    to[projection[i]] = from[i];
}
