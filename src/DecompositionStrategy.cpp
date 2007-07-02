#include "stdinc.h"
#include "DecompositionStrategy.h"

#include "TermTree.h"
#include "TermTranslator.h"
#include "Partition.h"
#include "monosIO.h"

DecompositionStrategy::DecompositionStrategy(ostream* out,
					     const VarNames& names,
					     unsigned int dimension,
					     const TermTranslator* translator,
					     bool doDegenerization):
  _firstPartition(dimension),
  _out(out),
  _dimension(dimension),
  _names(names),
  _first(true),
  _translator(translator),
  _doDegenerization(doDegenerization),
  _ioHandler(new MonosIOHandler()) {
  ASSERT(out != 0);
  
  _solutions.push(new TermCont());
  _ioHandler->startWritingIdeal(*_out, _names);
}

DecompositionStrategy::~DecompositionStrategy() {
  ASSERT(_solutions.size() == 1);
  
  _translator->getExponent(0,0);

  if (_doDegenerization)
    degenerisize();

  TermCont& sols = *(_solutions.top());
  sort(sols.begin(), sols.end());
  sols.erase(unique(sols.begin(), sols.end()), sols.end());
    
  for (TermCont::iterator it = sols.begin(); it != sols.end(); ++it)
    writeSolution(*it);
  delete _solutions.top();
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
      //	cout << "applying " << *it2 << " to " << *it1;
      for (int i = _dimension - 1; i >= 0; --i) {
	if (compressor[i] == 0xFFFFFFFF)
	  continue;
	(*it1)[i] = (*it2)[compressor[i]];
      }
      //	cout << " getting " << *it1 << endl;

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

  for (TermCont::iterator it = sols->begin();
       it != sols->end(); ++it)
    _solutions.top()->push_back(*it);

  delete sols;
}

void DecompositionStrategy::degenerisize() {
  TermTree decom(_dimension);
  TermCont& sols = *(_solutions.top());
  for (TermCont::iterator it = sols.begin(); it != sols.end(); ++it) {
    for (unsigned int var = 0; var < _dimension; ++var) {
      if (_translator->getExponent(var, (*it)[var] + 1) != 0) {
	while ((*it)[var] > 0 &&
	       _translator->getExponent(var, (*it)[var]) ==
	       _translator->getExponent(var, (*it)[var] + 1))
	  --((*it)[var]);
      }
    }

    if (!decom.getDominator(*it)) {
      decom.removeDivisors(*it);
      decom.insert(*it);
    }
  }

  sols.clear();
  TermTree::TreeIterator it = TermTree::TreeIterator(decom);
  Term term(_dimension);
  while (!it.atEnd()) {
    it.getTerm(term);
    ++it;
    sols.push_back(term);
  }
}

void DecompositionStrategy::writeSolution(const Term& b) {
  ASSERT(_dimension == b.getDimension());
  
  static vector<mpz_class> term(_dimension);
  for (unsigned int var = 0; var < _dimension; ++var)
    term[var] = (_translator->getExponent(var, b[var] + 1));
  _ioHandler->writeGeneratorOfIdeal(*_out, term, _names);
}
