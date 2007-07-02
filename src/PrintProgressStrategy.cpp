#include "stdinc.h"
#include "PrintProgressStrategy.h"

#include "TermTree.h"

#include <iomanip>

PrintProgressStrategy::PrintProgressStrategy():
  _workTotal(0),
  _workDone(0) {}

void PrintProgressStrategy::getName(string& name) const {
  name = "PrintProgressStrategy";
}

bool PrintProgressStrategy::consideringCall(const Term& b,
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

bool PrintProgressStrategy::startingCall(const Term& b,
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

void PrintProgressStrategy::endingCall(const Term& b,
				       const TermTree& tree) {
}

void PrintProgressStrategy::foundSolution(const Term& b,
					  bool startingPartition) {
}

void PrintProgressStrategy::startingPartitioning(const Term& b,
						 const Partition& partition,
						 const TermTree& tree) {
}

void PrintProgressStrategy::doingPartitionSet(int position,
					      const Term& b,
					      const Term& compressedB,
					      const Partition& partition,
					      vector<Exponent> compressor,
					      const TermTree& tree) {
}

void PrintProgressStrategy::
doneDoingPartitionSet(int position,
		      const vector<Exponent>& compressor) {
}

void PrintProgressStrategy::endingPartitioning(int position,
					       const Term& b) {
}
