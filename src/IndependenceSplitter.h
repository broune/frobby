#ifndef INDEPENDENCE_SPLITTER_GUARD
#define INDEPENDENCE_SPLITTER_GUARD

#include "DecomConsumer.h"
#include "Partition.h"
#include "Projection.h"
#include "Ideal.h"
#include <vector>
#include "Slice.h"

class Ideal;
class Term;

class IndependenceSplitter {
 public:
  // Slice must be simplified and normalized.
  IndependenceSplitter(const Partition& partition, Slice& slice);
  ~IndependenceSplitter();

  static void computePartition(Partition& partition, const Slice& slice);
  static bool shouldPerformSplit(const Partition& partition,
				 const Slice& slice);

  size_t getChildCount() const;
  Ideal* getMixedProjectionSubtract();

  Slice& getSlice(size_t part);
  Projection& getProjection(size_t part);

 private:
  struct Child {
    Slice slice;
    Projection projection;

    void swap(Child& child);
    bool operator<(const Child& child) const;
  };

  void initializeChildren(const Partition& partition);
  void populateChildIdealsAndSingletonDecom(const vector<Child*>& childAt);
  void populateChildSubtracts(const vector<Child*>& childAt);

  Slice& _slice;
  vector<Child> _children;
  Ideal* _mixedProjectionSubtract;
};

#endif
