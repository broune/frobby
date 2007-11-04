#include "../stdinc.h"
#include "CompositeStrategy.h"

#include "Strategy.h"
#include "TermTree.h"

#include "../TermTranslator.h"
#include "../VarNames.h"

CompositeStrategy::CompositeStrategy(Strategy* strategy1, Strategy* strategy2):
  _strategy1(strategy1),
  _strategy2(strategy2) {
  ASSERT(strategy1 != 0);
  ASSERT(strategy2 != 0);
}

CompositeStrategy::~CompositeStrategy() {
  delete _strategy1;
  delete _strategy2;
}

void CompositeStrategy::getName(string& name) const {
  name = "CompositeStrategy";
}

bool CompositeStrategy::consideringCall(const Term& b,
					bool sameExponentAsNext,
					const TermTree& tree) {
  bool iSayContinue1 =
    _strategy1->consideringCall(b, sameExponentAsNext, tree);
  bool iSayContinue2 =
    _strategy2->consideringCall(b, sameExponentAsNext, tree);
  return iSayContinue1 || iSayContinue2;
}

bool CompositeStrategy::startingCall(const Term& b,
				     const TermTree& tree,
				     bool startingPartition) {
  bool iSayContinue1 = _strategy1->startingCall(b, tree, startingPartition);
  bool iSayContinue2 = _strategy2->startingCall(b, tree, startingPartition);
  return iSayContinue1 || iSayContinue2;
}

void CompositeStrategy::endingCall(const Term& b,
				   const TermTree& tree) {
  _strategy1->endingCall(b, tree);
  _strategy2->endingCall(b, tree);
}

void CompositeStrategy::foundSolution(const Term& b,
				      bool startingPartition) {
  _strategy1->foundSolution(b, startingPartition);
  _strategy2->foundSolution(b, startingPartition);
}

void CompositeStrategy::startingPartitioning(const Term& b,
					     const OldPartition& partition,
					     const TermTree& tree) {
  _strategy1->startingPartitioning(b, partition, tree);
  _strategy2->startingPartitioning(b, partition, tree);
}

void CompositeStrategy::doingPartitionSet(int position,
					  const Term& b,
					  const Term& compressedB,
					  const OldPartition& partition,
					  vector<Exponent> compressor,
					  const TermTree& tree) {
  _strategy1->
    doingPartitionSet(position, b, compressedB, partition, compressor, tree);
  _strategy2->
    doingPartitionSet(position, b, compressedB, partition, compressor, tree);
}

void CompositeStrategy::
doneDoingPartitionSet(int position,
		      const vector<Exponent>& compressor) {
  _strategy1->doneDoingPartitionSet(position, compressor);
  _strategy2->doneDoingPartitionSet(position, compressor);
}

void CompositeStrategy::endingPartitioning(int position,
					   const Term& b) {
  _strategy1->endingPartitioning(position, b);
  _strategy2->endingPartitioning(position, b);
}
