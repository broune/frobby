#ifndef SKIP_REDUNDANT_STRATEGY_GUARD
#define SKIP_REDUNDANT_STRATEGY_GUARD

#include "Strategy.h"

class TermTree;
class TermTranslator;
class OldPartition;

class SkipRedundantStrategy : public Strategy {
public:
  SkipRedundantStrategy(Strategy* strategy,
			unsigned int dimension);
  ~SkipRedundantStrategy();

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

  void print(ostream& out);

private:
  vector<TermTree> _skip;
  vector<vector<Term> > _newSkip;
  vector<bool> _remember;
  unsigned int _dimension;
  Strategy* _strategy;

  vector<Exponent> _toAddNext;
  vector<TermTree> _seen;
  vector<TermTree> _projectedSeen;
  vector<bool> _seenHasChanged;
};

#endif
