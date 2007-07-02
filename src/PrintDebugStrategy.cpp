#include "stdinc.h"
#include "PrintDebugStrategy.h"

#include "TermTree.h"

#include <iterator>

PrintDebugStrategy::~PrintDebugStrategy() {
  cerr << "The computation is done." << endl;
}

void PrintDebugStrategy::getName(string& name) const {
  name = "PrintDebugStrategy";
}

bool PrintDebugStrategy::consideringCall(const ExternalTerm& b,
					 bool sameExponentAsNext,
					 const TermTree& tree) {
  return false;
}

bool PrintDebugStrategy::startingCall(const ExternalTerm& b,
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

void PrintDebugStrategy::endingCall(const ExternalTerm& b,
				    const TermTree& tree) {
  for (int i = 0; i < tree.getPosition(); ++i)
    cerr << 'v';
  cerr << " Ending b=" << b
       << " position=" << tree.getPosition() << endl;
}

void PrintDebugStrategy::foundSolution(const ExternalTerm& b,
				       bool startingPartition) {
  for (unsigned int i = 0; i < b.getDimension(); ++i)
    cerr << '*';
  cerr << " Found solution " << b << endl;
}

void PrintDebugStrategy::startingPartitioning(const ExternalTerm& b,
					      const Partition& partition,
					      const TermTree& tree) {
  fill_n(ostream_iterator<char>(cerr), tree.getPosition(), '-');
  cerr << " starting partitioning" << endl;
}

void PrintDebugStrategy::doingPartitionSet(int position,
					   const ExternalTerm& b,
					   const ExternalTerm& compressedB,
					   const Partition& partition,
					   vector<Exponent> compressor,
					   const TermTree& tree) {
  cerr << "doing a set of the partition" << endl;
}

void PrintDebugStrategy::doneDoingPartitionSet
(int position,
 const vector<Exponent>& compressor) {
  cerr << "done doing that set of the partition" << endl;
}

void PrintDebugStrategy::endingPartitioning(int position,
					    const ExternalTerm& b) {
  cerr << "ending partitioning" << endl;
}
