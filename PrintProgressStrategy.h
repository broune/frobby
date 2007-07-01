#ifndef PRINT_PROGRESS_STRATEGY_GUARD
#define PRINT_PROGRESS_STRATEGY_GUARD

#include "Strategy.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"
#include "Partition.h"

#include <iomanip>

class PrintProgressStrategy : public Strategy {
public:
  PrintProgressStrategy():
    _workTotal(0),
    _workDone(0) {}
  virtual ~PrintProgressStrategy() {}

  virtual void getName(string& name) const {
    name = "PrintProgressStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    ASSERT(_workTotal != 0);
    ASSERT(_workDone <= _workTotal);

    if (tree.getPosition() == 0) {
      if (_timeSinceLastReport.getSeconds() >= 5) {
	_timeSinceLastReport.reset();

	double doneRatio = ((double)_workDone)/_workTotal;
	cerr << setprecision(3)
	     << _workDone << '/' << _workTotal << '='
	     << doneRatio * 100.0 << "% done in " << _totalTime << '.' << endl;
      }

      ++_workDone;
    }

    return false;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    if (tree.getPosition() == 0) {
      _workTotal = 0;
      TermTree::TreeIterator treeIt(tree);
      while (!treeIt.atEnd()) {
	if (treeIt.getExponent(0) != 0)
	  ++_workTotal;
	++treeIt;
      }
    }

    return false;
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

private:
  Timer _totalTime;
  Timer _timeSinceLastReport;
  unsigned int _workTotal;
  unsigned int _workDone;
};

#endif
