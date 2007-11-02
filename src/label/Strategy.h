#ifndef STRATEGY_GUARD
#define STRATEGY_GUARD

class Term;
class TermTree;
class OldPartition;

class Strategy {
public:
  virtual ~Strategy();

  virtual bool consideringCall(const Term& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) = 0;

  virtual bool startingCall(const Term& b,
			    const TermTree& tree,
			    bool startingPartition) = 0;

  virtual void endingCall(const Term& b,
			  const TermTree& tree) = 0;

  virtual void foundSolution(const Term& b,
			     bool startingPartition) = 0;

  virtual void startingPartitioning(const Term& b,
				    const OldPartition& partition,
				    const TermTree& tree);
    
  virtual void doingPartitionSet(int position,
				 const Term& b,
				 const Term& compressedB,
				 const OldPartition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree);

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor);

  virtual void endingPartitioning(int position,
				  const Term& b);

  virtual void getName(string& name) const = 0;
};

#endif
