#ifndef BENCHMARK_STRATEGY
#define BENCHMARK_STRATEGY

#include "Strategy.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"
#include "Partition.h"

class BenchmarkStrategy : public Strategy {
public:
  BenchmarkStrategy() {}
  virtual ~BenchmarkStrategy() {}

  virtual void getName(string& name) const {
    name = "BenchmarkStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    return true;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    return true;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
  }  

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
  }
};

#endif
