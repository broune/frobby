#include "../stdinc.h"
#include "StatisticsStrategy.h"

#include "TermTree.h"

StatisticsStrategy::StatisticsStrategy(int dimension):
  _dimension(dimension),
  _callCounts(dimension) {
}

StatisticsStrategy::~StatisticsStrategy() {
  for (unsigned int position = 0; position < _dimension; ++position)
    fprintf(stderr, "Level %u had %u calls.\n",
	    position + 1, _callCounts[position]);
}

void StatisticsStrategy::getName(string& name) const {
  name = "StatisticsStrategy";
}

bool StatisticsStrategy::consideringCall(const Term& b,
					 bool sameExponentAsNext,
					 const TermTree& tree) {
  return false;
}

bool StatisticsStrategy::startingCall(const Term& b,
				      const TermTree& tree,
				      bool startingPartition) {
  return false;
}

void StatisticsStrategy::endingCall(const Term& b,
				    const TermTree& tree) {
  ++(_callCounts[tree.getPosition()]);
}

void StatisticsStrategy::foundSolution(const Term& b,
				       bool startingPartition) {
  ++(_callCounts[_dimension - 1]);
}

void StatisticsStrategy::startingPartitioning(const Term& b,
					      const Partition& partition,
					      const TermTree& tree) {
}
    
void StatisticsStrategy::doingPartitionSet(int position,
					   const Term& b,
					   const Term& compressedB,
					   const Partition& partition,
					   vector<Exponent> compressor,
					   const TermTree& tree) {
}

void StatisticsStrategy::
doneDoingPartitionSet(int position,
		      const vector<Exponent>& compressor) {
}

void StatisticsStrategy::endingPartitioning(int position,
					    const Term& b) {
}
