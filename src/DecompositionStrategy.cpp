#include "stdinc.h"
#include "DecompositionStrategy.h"

#include "TermTree.h"
#include "TermTranslator.h"
#include "Partition.h"
#include "monosIO.h"

DecompositionStrategy::DecompositionStrategy(ostream* out,
					     const VarNames& names,
					     unsigned int dimension,
					     const TermTranslator* translator):
  _firstPartition(dimension),
  _out(out),
  _dimension(dimension),
  _names(names),
  _first(true),
  _translator(translator),
  _ioHandler(new MonosIOHandler()),
  _outputTmp(dimension) {
  ASSERT(out != 0);
  
  _solutions.push(new TermCont());
  _ioHandler->startWritingIdeal(*_out, _names);
}

DecompositionStrategy::~DecompositionStrategy() {
  ASSERT(_solutions.size() == 1);

  flushIfPossible();

  _solutions.pop();
  ASSERT(_solutions.empty());
    
  _ioHandler->doneWritingIdeal(*_out);
  _out->flush();
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
						 const Partition& partition,
						 const TermTree& tree) {
  _firstPartition[tree.getPosition()] = true;
  _solutions.push(new TermCont());
  _solutions.top()->push_back(b);
}

void DecompositionStrategy::doingPartitionSet(int position,
					      const Term& b,
					      const Term& compressedB,
					      const Partition& partition,
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
  ASSERT(_dimension == b.getDimension());

  for (unsigned int var = 0; var < _dimension; ++var)
    _outputTmp[var] = (_translator->getExponentString(var, b[var] + 1));
    _ioHandler->writeGeneratorOfIdeal(*_out, _outputTmp, _names);
}
