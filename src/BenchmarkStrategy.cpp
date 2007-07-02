#include "stdinc.h"
#include "BenchmarkStrategy.h"

void BenchmarkStrategy::getName(string& name) const {
  name = "BenchmarkStrategy";
}

bool BenchmarkStrategy::consideringCall(const Term& b,
					bool sameExponentAsNext,
					const TermTree& tree) {
  return true;
}

bool BenchmarkStrategy::startingCall(const Term& b,
				     const TermTree& tree,
				     bool startingPartition) {
  return true;
}

void BenchmarkStrategy::endingCall(const Term& b,
				   const TermTree& tree) {
}

void BenchmarkStrategy::foundSolution(const Term& b,
				      bool startingPartition) {
}  

void BenchmarkStrategy::startingPartitioning(const Term& b,
					     const Partition& partition,
					     const TermTree& tree) {
}

void BenchmarkStrategy::doingPartitionSet(int position,
					  const Term& b,
					  const Term& compressedB,
					  const Partition& partition,
					  vector<Exponent> compressor,
					  const TermTree& tree) {
}

void BenchmarkStrategy::doneDoingPartitionSet(int position,
					      const vector<Exponent>& compressor) {
}

void BenchmarkStrategy::endingPartitioning(int position,
					   const Term& b) {
}
