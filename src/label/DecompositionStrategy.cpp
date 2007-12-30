#include "../stdinc.h"
#include "DecompositionStrategy.h"

#include "TermTree.h"
#include "OldPartition.h"

#include "../TermConsumer.h"

DecompositionStrategy::DecompositionStrategy(TermConsumer* consumer,
					     unsigned int dimension):
  _dimension(dimension),
  _firstPartition(dimension),
  _consumer(consumer) {
  _solutions.push(new TermCont());
}

DecompositionStrategy::~DecompositionStrategy() {
  ASSERT(_solutions.size() == 1);

  flushIfPossible();

  delete _solutions.top();
  _solutions.pop();
  ASSERT(_solutions.empty());

  delete _consumer;
}

void DecompositionStrategy::getName(string& name) const {
  name = "DecompositionStrategy";
}

bool DecompositionStrategy::consideringCall(const Term& b,
					    bool sameExponentAsNext,
					    const TermTree& tree) {
  return true;
}

bool DecompositionStrategy::startingCall(const Term& b,
					 const TermTree& tree,
					 bool startingPartition) {
  return true;
}

void DecompositionStrategy::endingCall(const Term& b,
				       const TermTree& tree) {
}

void DecompositionStrategy::foundSolution(const Term& b,
					  bool startingPartition) {
  if (_solutions.size() == 1)
    writeSolution(b);
  else
    _solutions.top()->push_back(b);
}

void DecompositionStrategy::startingPartitioning(const Term& b,
						 const OldPartition& partition,
						 const TermTree& tree) {
  _firstPartition[tree.getPosition()] = true;
  _solutions.push(new TermCont());
  _solutions.top()->push_back(b);
}

void DecompositionStrategy::doingPartitionSet(int position,
					      const Term& b,
					      const Term& compressedB,
					      const OldPartition& partition,
					      vector<Exponent> compressor,
					      const TermTree& tree) {
  _solutions.push(new TermCont());
}

void DecompositionStrategy::
doneDoingPartitionSet(int position,
		      const vector<Exponent>& compressor) {
  TermCont* sols2 = _solutions.top();
  _solutions.pop();

  TermCont* sols1 = _solutions.top();
  _solutions.pop();

  TermCont* sols = new TermCont();

  for (TermCont::iterator it1 = sols1->begin();
       it1 != sols1->end(); ++it1) {
    for (TermCont::iterator it2 = sols2->begin();
	 it2 != sols2->end(); ++it2) {
      for (int i = _dimension - 1; i >= 0; --i) {
	if (compressor[i] == 0xFFFFFFFF)
	  continue;
	(*it1)[i] = (*it2)[compressor[i]];
      }

      sols->push_back(*it1);
    }
  }
    
  delete sols1;
  delete sols2;

  _solutions.push(sols);
}

void DecompositionStrategy::endingPartitioning(int position,
					       const Term& b) {
  TermCont* sols = _solutions.top();
  _solutions.pop();

  if (_solutions.size() != 1) {
    for (TermCont::iterator it = sols->begin();
	 it != sols->end(); ++it)
      _solutions.top()->push_back(*it);
  } else {
    for (TermCont::iterator it = sols->begin();
	 it != sols->end(); ++it)
      writeSolution(*it);
  }

  delete sols;

  flushIfPossible();
}

void DecompositionStrategy::flushIfPossible() {
  if (_solutions.size() != 1)
    return;
  
  TermCont& sols = *(_solutions.top());
    
  for (TermCont::iterator it = sols.begin(); it != sols.end(); ++it)
    writeSolution(*it);
  _solutions.top()->clear();
}

void DecompositionStrategy::writeSolution(const Term& b) {
  ASSERT(_dimension == b.getVarCount());

  Term& t = *(const_cast<Term*>(&b));
  for (size_t var = 0; var < b.getVarCount(); ++var)
    t[var] += 1;
  _consumer->consume(t);
  for (size_t var = 0; var < b.getVarCount(); ++var)
    t[var] -= 1;
}
