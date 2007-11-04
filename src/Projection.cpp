#include "stdinc.h"
#include "Projection.h"

#include "Term.h"
#include "Partition.h"


size_t Projection::getRangeVarCount() const {
  return _offsets.size();
}

void Projection::reset(const Partition& partition,
		       int number) {
  _offsets.resize(partition.getSetSize(number));

  size_t root = 0xFFFFFFFF;
  for (size_t i = 0; i < partition.getSize(); ++i) {
    if (i == partition.getRoot(i)) {
      if (number == 0) {
	root = i;
	break;
      }
      --number;
    }
  }
  ASSERT(number == 0 && root != 0xFFFFFFFF);

  size_t projectionOffset = 0;

  for (size_t i = 0; i < partition.getSize(); ++i) {
    if (partition.getRoot(i) != root)
      continue;

    _offsets[projectionOffset] = i;
    ++projectionOffset;
  }
}

void Projection::project(Term& to, const Exponent* from) const {
  size_t size = _offsets.size();
  for (size_t i = 0; i < size; ++i)
    to[i] = from[_offsets[i]];
}

void Projection::inverseProject(Term& to, const Exponent* from) const {
  size_t size = _offsets.size();
  for (size_t i = 0; i < size; ++i)
    to[_offsets[i]] = from[i];
}
