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

  // Runs the algorithm and deletes the ideal, the consumer and the
  // strategy.
  void runAndDeleteIdealAndReset(Ideal* ideal);

 private:
  void content(Slice& slice, bool simplifiedAndDependent = false);
  void labelSplit(Slice& slice);
  void pivotSplit(Slice& slice);
  bool independenceSplit(Slice& slice);
  
  DecomConsumer* _decomConsumer;
  SliceStrategy* _strategy;
};

#endif
