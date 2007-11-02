#ifndef INDEPENDENCE_SPLITTER_GUARD
#define INDEPENDENCE_SPLITTER_GuARD

#include "Partition.h"
#include <vector>

class Slice;
class Ideal;
class Term;

class IndependenceSplitter {
  IndependenceSplitter(Slice& slice);

  bool shouldPerformSplit() const;

  size_t getChildCount() const;

  Slice* makeChild(size_t number);
  void addToChildDecom(size_t number, const Term& component);

  void generateDecom(DecomConsumer* consumer);

 private:
  Slice& _slice;
  Partition _partition;
  vector<Ideal*> _childDecoms;
};

#endif
