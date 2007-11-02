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

class SliceAlgorithm {
 public:
  SliceAlgorithm(const Ideal& ideal,
		 const VarNames& names,
		 const TermTranslator* translator,
		 ostream& out);

 private:
  void content(Slice& slice, bool simplifiedAndDependent = false);
  void labelSplit(Slice& slice);
  void pivotSplit(Slice& slice);
  bool independenceSplit(Slice& slice);
  
  struct ProjDecom {
    ProjDecom(): decom(0) {}
    ~ProjDecom();
    vector<Exponent> compressor;
    Ideal* decom;
  };
  void inverseDecomProject(const vector<ProjDecom>& projs,
			   Partition& partition,
			   size_t proj,
			   Term& term);

  void getPivot(const Slice& slice, Term& pivot);

  DecomConsumer* _decomConsumer;
};

#endif
