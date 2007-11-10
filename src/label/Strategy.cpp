#include "../stdinc.h"
#include "Strategy.h"

#include "PrintDebugStrategy.h"
#include "StatisticsStrategy.h"
#include "CompositeStrategy.h"
#include "PrintProgressStrategy.h"
#include "SkipRedundantStrategy.h"

Strategy::~Strategy() {
}

void Strategy::startingPartitioning(const Term& b,
				    const OldPartition& partition,
				    const TermTree& tree) {
  ASSERT(false);
}
    
void Strategy::doingPartitionSet(int position,
				 const Term& b,
				 const Term& compressedB,
				 const OldPartition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
  ASSERT(false);
}

void Strategy::doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
  string name;
  getName(name);
  cerr << "ERROR: doneDoingPartitionSet not implemented in " << name << "." << endl;
  ASSERT(false);
}

void Strategy::endingPartitioning(int position,
				  const Term& b) {
  string name;
  getName(name);
  cerr << "ERROR: endingPartitioning not implemented in " << name << "." << endl;
  ASSERT(false);
}

Strategy* Strategy::addDebugOutput(Strategy* strategy) {
  return new CompositeStrategy(strategy, new PrintDebugStrategy());
}

Strategy* Strategy::addStatistics(Strategy* strategy, size_t varCount) {
  return new CompositeStrategy(strategy, new StatisticsStrategy(varCount));
}

Strategy* Strategy::addPrintProgress(Strategy* strategy) {
  return new CompositeStrategy(strategy, new PrintProgressStrategy());
}

Strategy* Strategy::addSkipRedundant(Strategy* strategy, size_t varCount) {
  return new SkipRedundantStrategy(strategy, varCount);
}
