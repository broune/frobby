#ifndef SLICE_ALGORITHM_GUARD
#define SLICE_ALGORITHM_GUARD

#include "Term.h"
#include "VarNames.h"
#include <ostream>

class Ideal;
class Term;
class IOHandler;
class TermTranslator;
class Partition;
class Slice;
class DecomConsumer;
class SliceStrategy;

class SliceAlgorithm {
 public:
  SliceAlgorithm();

  // These take over ownership of the parameter.
  void setConsumer(DecomConsumer* consumer);
  void setStrategy(SliceStrategy* strategy);
  void setUseIndependence(bool useIndependence);

  // Runs the algorithm and clears ideal. Then deletes the strategy
  // and consumer.
  void runAndClear(Ideal& ideal);

 private:
  void content(Slice& slice, bool simplifiedAndDependent = false);
  void labelSplit(Slice& slice);
  void labelSplit2(Slice& slice);
  void pivotSplit(Slice& slice);
  bool independenceSplit(Slice& slice);

  bool _useIndependence;
  DecomConsumer* _decomConsumer;
  SliceStrategy* _strategy;
};

#endif
