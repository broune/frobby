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

int Partition::getSetCount(int minSize, int position) const {
  int partitionCount = 0;
  for (int i = position; i < _size; ++i)
    if (i == getRoot(i) &&
	-_partitions[i] >= minSize)
      ++partitionCount;
  return partitionCount;
}

int Partition::getSetSize(int set, int position) const {
  for (int i = position; i < _size; ++i) {
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

void Partition::getSetTranslators(int number,
				  vector<Exponent>& compressor,
				  vector<Exponent>& decompressor,
				  int position) const {
  
  int root = -1;
  for (int i = position; i < _size; ++i) {
    if (i == getRoot(i)) {
      if (number == 0) {
	root = i;
	break;
      }
      --number;
    }
  }
  ASSERT(number == 0 && root != -1);

  compressor.resize(_size);
  decompressor.resize(_size);
    
  // Compressing t is done by setting t[compressor[i]] = t[i].

  int decompressorOffset = decompressor.size() - 1;
  int compressorOffset = compressor.size() - 1;
  for (; compressorOffset >= 0; --compressorOffset) {
    if (getRoot(compressorOffset) != root) {
      compressor[compressorOffset] = 0xFFFFFFFF;
      continue;
    }

    compressor[compressorOffset] = decompressorOffset;
    //decompressor[decompressorOffset] = compressorOffset;
    --decompressorOffset;
  }
}

bool Partition::compress(Term& term,
			 const vector<Exponent>& compressor) const {
  bool insideSet = false;
  bool outsideSet = false;

  for (int i = _size - 1; i >= 0; --i) {
    if (compressor[i] == 0xFFFFFFFF) {
      if (term[i] != 0)
	outsideSet = true;
      continue;
    }
    if (term[i] != 0)
      insideSet = true;
    term[compressor[i]] = term[i];
  }

  return insideSet && !outsideSet;
}

void Partition::print(ostream& out) const {
  out << "Partition(size=" << _size << " sets:";
  for (int i = 0; i < _size; ++i)
    out << ' ' << _partitions[i];
  out << endl;
}

void Partition::project(Term& to, const Exponent* from,
	     const vector<Exponent>& compressor) const {
  size_t dummies = 0;
  for (int i = _size - 1; i >= 0; --i)
    if (compressor[i] == 0xFFFFFFFF)
      ++dummies;

  for (int i = _size - 1; i >= 0; --i)
    if (compressor[i] != 0xFFFFFFFF)
      to[compressor[i] - dummies] = from[i];
}

void Partition::inverseProject(Term& to, const Exponent* from,
	     const vector<Exponent>& compressor) const {
  size_t dummies = 0;
  for (int i = _size - 1; i >= 0; --i)
    if (compressor[i] == 0xFFFFFFFF)
      ++dummies;

  for (int i = _size - 1; i >= 0; --i)
    if (compressor[i] != 0xFFFFFFFF)
      to[i] = from[compressor[i] - dummies];
}
