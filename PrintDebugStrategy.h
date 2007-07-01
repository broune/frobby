#ifndef PRINT_DEBUG_STRATEGY_GUARD
#define PRINT_DEBUG_STRATEGY_GUARD

#include "Strategy.h"

#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"
#include "Partition.h"

class PrintDebugStrategy : public Strategy {
public:
  PrintDebugStrategy() {}
  virtual ~PrintDebugStrategy() {
    cerr << "The computation is done." << endl;
  }

  virtual void getName(string& name) const {
    name = "PrintDebugStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    return false;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    if (tree.getPosition() == 0) {
      cerr << "Debug printing on. Starting the computation." << endl;
    }

    for (int i = 0; i < tree.getPosition(); ++i)
      cerr << '^';
    cerr << " Starting b=" << b
	 << " position=" << tree.getPosition() << endl;

    return false;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    for (int i = 0; i < tree.getPosition(); ++i)
      cerr << 'v';
    cerr << " Ending b=" << b
	 << " position=" << tree.getPosition() << endl;
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    for (unsigned int i = 0; i < b.getDimension(); ++i)
      cerr << '*';
    cerr << " Found solution " << b << endl;
  }

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    fill_n(ostream_iterator<char>(cerr), tree.getPosition(), '-');
    cerr << " starting partitioning" << endl;
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    cerr << "doing a set of the partition" << endl;
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
    cerr << "done doing that set of the partition" << endl;
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    cerr << "ending partitioning" << endl;
  }
};

#endif
