#ifndef COMPOSITE_STRATEGY_GUARD
#define COMPOSITE_STRATEGY_GUARD

#include "Strategy.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"
#include "Partition.h"

class CompositeStrategy : public Strategy {
public:
  CompositeStrategy(Strategy* strategy1, Strategy* strategy2):
    _strategy1(strategy1),
    _strategy2(strategy2) {
    ASSERT(strategy1 != 0);
    ASSERT(strategy2 != 0);
  }

  virtual ~CompositeStrategy() {}

  virtual void getName(string& name) const {
    name = "CompositeStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    bool iSayContinue1 = _strategy1->consideringCall(b, sameExponentAsNext, tree);
    bool iSayContinue2 = _strategy2->consideringCall(b, sameExponentAsNext, tree);
    return iSayContinue1 || iSayContinue2;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    bool iSayContinue1 = _strategy1->startingCall(b, tree, startingPartition);
    bool iSayContinue2 = _strategy2->startingCall(b, tree, startingPartition);
    return iSayContinue1 || iSayContinue2;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    _strategy1->endingCall(b, tree);
    _strategy2->endingCall(b, tree);
  }
  
  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    _strategy1->foundSolution(b, startingPartition);
    _strategy2->foundSolution(b, startingPartition);
  }


  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    _strategy1->startingPartitioning(b, partition, tree);
    _strategy2->startingPartitioning(b, partition, tree);
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    _strategy1->doingPartitionSet(position, b, compressedB, partition, compressor, tree);
    _strategy2->doingPartitionSet(position, b, compressedB, partition, compressor, tree);
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
    _strategy1->doneDoingPartitionSet(position, compressor);
    _strategy2->doneDoingPartitionSet(position, compressor);
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    _strategy1->endingPartitioning(position, b);
    _strategy2->endingPartitioning(position, b);
  }

private:
  Strategy* _strategy1;
  Strategy* _strategy2;
};

#endif
