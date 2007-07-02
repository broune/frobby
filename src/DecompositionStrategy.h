#ifndef DECOMPOSITION_STRATEGY
#define DECOMPOSITION_STRATEGY

#include <stack>

#include "Strategy.h"
#include "VarNames.h"

class TermTree;
class TermTranslator;
class Partition;
class IOHandler;

class DecompositionStrategy : public Strategy {
  typedef vector<Term> TermCont;

public:
  DecompositionStrategy(ostream* out,
			const VarNames& names,
			unsigned int dimension,
			const TermTranslator* translator,
			bool doDegenerization);

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

protected:
  void degenerisize();

  void writeSolution(const Term& b);

  stack<TermCont*> _solutions;

  vector<bool> _firstPartition;

  ostream* _out;
  unsigned int _dimension;
  VarNames _names;
  bool _first;
  const TermTranslator* _translator;
  bool _doDegenerization;
  IOHandler* _ioHandler;
};

#endif
