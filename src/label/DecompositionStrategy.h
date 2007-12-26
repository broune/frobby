#ifndef DECOMPOSITION_STRATEGY
#define DECOMPOSITION_STRATEGY

#include "Strategy.h"
#include "../VarNames.h"

#include <list>
#include <stack>

class TermTree;
class TermTranslator;
class OldPartition;
class IOHandler;
class TermConsumer;

class DecompositionStrategy : public Strategy {
  typedef list<Term> TermCont;

public:
  // Ownership of decomConsumer is taken over.
  DecompositionStrategy(TermConsumer* consumer,
			unsigned int dimension);

  virtual ~DecompositionStrategy();

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

protected:
  void writeSolution(const Term& b);
  void flushIfPossible();


  unsigned int _dimension;

  stack<TermCont*> _solutions;
  vector<bool> _firstPartition;

  TermConsumer* _consumer;
};

#endif
