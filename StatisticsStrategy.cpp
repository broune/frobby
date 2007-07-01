#include "stdinc.h"
#include "StatisticsStrategy.h"

#include "TermTree.h"

StatisticsStrategy::StatisticsStrategy(int dimension):
  _dimension(dimension),
  _callCounts(dimension) {
}

StatisticsStrategy::~StatisticsStrategy() {
  for (unsigned int position = 0; position < _dimension; ++position)
    cerr << "Level " << position + 1 << " had "
	 << _callCounts[position] << " calls. " << endl;
}

void StatisticsStrategy::getName(string& name) const {
  name = "StatisticsStrategy";
}

bool StatisticsStrategy::consideringCall(const ExternalTerm& b,
					 bool sameExponentAsNext,
					 const TermTree& tree) {
  return false;
}

bool StatisticsStrategy::startingCall(const ExternalTerm& b,
				      const TermTree& tree,
				      bool startingPartition) {
  return false;
}

void StatisticsStrategy::endingCall(const ExternalTerm& b,
				    const TermTree& tree) {
  ++(_callCounts[tree.getPosition()]);
}

void StatisticsStrategy::foundSolution(const ExternalTerm& b,
				       bool startingPartition) {
  ++(_callCounts[_dimension - 1]);
}

void StatisticsStrategy::startingPartitioning(const ExternalTerm& b,
					      const Partition& partition,
					      const TermTree& tree) {
}
    
void StatisticsStrategy::doingPartitionSet(int position,
					   const ExternalTerm& b,
					   const ExternalTerm& compressedB,
					   const Partition& partition,
					   vector<Exponent> compressor,
					   const TermTree& tree) {
}

void StatisticsStrategy::
doneDoingPartitionSet(int position,
		      const vector<Exponent>& compressor) {
}

void StatisticsStrategy::endingPartitioning(int position,
					    const ExternalTerm& b) {
}
