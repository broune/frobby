#ifndef SLICE_ALGORITHM_GUARD
#define SLICE_ALGORITHM_GUARD

class Ideal;
class Slice;
class SliceStrategy;
class Term;

// Computes the Maximal Standard Monomials (MSMs) of the input ideal.
class SliceAlgorithm {
 public:
  SliceAlgorithm();

  // setStrategy takes over ownership of the strategy.
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
  SliceStrategy* _strategy;
};

// For getting a single MSM. This may fail if slice.getIdeal() is
// not artinian, in which case the return value is false.
bool computeSingleMSM(const Slice& slice, Term& msm);

#endif
