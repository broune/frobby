#ifndef INDEPENDENCE_SPLITTER_GUARD
#define INDEPENDENCE_SPLITTER_GUARD

#include "DecomConsumer.h"
#include "Partition.h"
#include "Projection.h"
#include <vector>

class Slice;
class Ideal;
class Term;

class IndependenceSplitter : public DecomConsumer {
 public:
  IndependenceSplitter(const Partition& partition, Slice& slice);

  static void computePartition(Partition& partition, const Slice& slice);
  static bool shouldPerformSplit(const Partition& partition,
				 const Slice& slice);

  size_t getChildCount() const;

  void setCurrentChild(size_t child, Slice& slice);

  void generateDecom(DecomConsumer* consumer);

  void consume(const Term& term);

 private:
  void generateDecom(DecomConsumer* consumer,
		     size_t child,
		     Term& partial);

  struct Child {
    Child(): ideal(0) {}

    Ideal* ideal; // TODO: rename to decom
    Projection projection;
  };

  Slice& _slice;
  vector<Child> _children;
  size_t _childCount;
  bool _anySingletons;
  bool _shouldSplit;
  size_t _currentChild;
};

#endif
