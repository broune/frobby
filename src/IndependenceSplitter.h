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

class IndependenceSplitter : public DecomConsumer {
 public:
  // Slice must be simplified and normalized.
  IndependenceSplitter(const Partition& partition, Slice& slice);
  ~IndependenceSplitter();

  static void computePartition(Partition& partition, const Slice& slice);
  static bool shouldPerformSplit(const Partition& partition,
				 const Slice& slice);

  size_t getChildCount() const;

  // It is only allowed to set the current child to each value once.
  void setCurrentChild(size_t child, Slice& slice);
  bool currentChildDecomIsEmpty() const;

  void generateDecom(DecomConsumer* consumer);

  void consume(const Term& term);

 private:
  struct Child {
    Ideal decom;
    Slice slice;
    Projection projection;

    bool operator<(const Child& child) const;
  };

  void initializeChildren(const Partition& partition);
  void populateChildIdealsAndSingletonDecom(const vector<Child*>& childAt);
  void populateChildSubtracts(const vector<Child*>& childAt);
  
  void generateDecom(DecomConsumer* consumer,
		     size_t child,
		     Term& partial);

  Slice& _slice;
  vector<Child> _children;
  size_t _childCount;
  bool _shouldSplit;
  size_t _currentChild;
  Term _singletonDecom;
  Ideal* _mixedProjectionSubtract;
};

#endif
