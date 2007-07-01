#include "stdinc.h"
#include "BenchmarkStrategy.h"

void BenchmarkStrategy::getName(string& name) const {
  name = "BenchmarkStrategy";
}

bool BenchmarkStrategy::consideringCall(const ExternalTerm& b,
					bool sameExponentAsNext,
					const TermTree& tree) {
  return true;
}

bool BenchmarkStrategy::startingCall(const ExternalTerm& b,
				     const TermTree& tree,
				     bool startingPartition) {
  return true;
}

void BenchmarkStrategy::endingCall(const ExternalTerm& b,
				   const TermTree& tree) {
}

void BenchmarkStrategy::foundSolution(const ExternalTerm& b,
				      bool startingPartition) {
}  

void BenchmarkStrategy::startingPartitioning(const ExternalTerm& b,
					     const Partition& partition,
					     const TermTree& tree) {
}

void BenchmarkStrategy::doingPartitionSet(int position,
					  const ExternalTerm& b,
					  const ExternalTerm& compressedB,
					  const Partition& partition,
					  vector<Exponent> compressor,
					  const TermTree& tree) {
}

void BenchmarkStrategy::doneDoingPartitionSet(int position,
					      const vector<Exponent>& compressor) {
}

void BenchmarkStrategy::endingPartitioning(int position,
					   const ExternalTerm& b) {
}
