#ifndef PRINT_PROGRESS_STRATEGY_GUARD
#define PRINT_PROGRESS_STRATEGY_GUARD

#include "Strategy.h"
#include "../Timer.h"

class TermTree;
class TermTranslator;
class Partition;

class PrintProgressStrategy : public Strategy {
public:
  PrintProgressStrategy();

  virtual void getName(string& name) const;

  virtual bool consideringCall(const Term& b,
			       bool sameExponentAsNext,
			       const TermTree& tree);

  virtual bool startingCall(const Term& b,
			    const TermTree& tree,
			    bool startingPartition);

  virtual void endingCall(const Term& b,
			  const TermTree& tree);

  virtual void foundSolution(const Term& b,
			     bool startingPartition);

  virtual void startingPartitioning(const Term& b,
				    const Partition& partition,
				    const TermTree& tree);
    
  virtual void doingPartitionSet(int position,
				 const Term& b,
				 const Term& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree);

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor);

  virtual void endingPartitioning(int position,
				  const Term& b);

private:
  Timer _totalTime;
  Timer _timeSinceLastReport;
  unsigned int _workTotal;
  unsigned int _workDone;
};

#endif
