#ifndef STRATEGY_GUARD
#define STRATEGY_GUARD

class ExternalTerm;
class TermTree;
class Partition;

class Strategy {
public:
  virtual ~Strategy();

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) = 0;

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) = 0;

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) = 0;

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) = 0;

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

  virtual void getName(string& name) const = 0;
};

#endif
