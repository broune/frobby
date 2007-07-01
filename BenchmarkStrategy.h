#ifndef BENCHMARK_STRATEGY
#define BENCHMARK_STRATEGY

#include "Strategy.h"

class TermTree;
class TermTranslator;
class VarNames;

class BenchmarkStrategy : public Strategy {
public:

  virtual void getName(string& name) const;

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree);

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition);

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree);

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition);

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree);
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree);

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor);

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b);
};

#endif
