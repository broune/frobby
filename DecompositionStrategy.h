#ifndef DECOMPOSITION_STRATEGY
#define DECOMPOSITION_STRATEGY

#include "Strategy.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"
#include "Partition.h"
#include "monosIO.h"
#include <stack>

class DecompositionStrategy : public Strategy {
  typedef vector<ExternalTerm> TermCont;

public:
  DecompositionStrategy(ostream* out,
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

  virtual ~DecompositionStrategy() {
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

  virtual void getName(string& name) const {
    name = "DecompositionStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    return true;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    return true;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    _solutions.top()->push_back(b);
  }


  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    _firstPartition[tree.getPosition()] = true;
    _solutions.push(new TermCont());
    _solutions.top()->push_back(b);
  }

  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    _solutions.push(new TermCont());
  }

  virtual void doneDoingPartitionSet(int position,
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

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    TermCont* sols = _solutions.top();
    _solutions.pop();

    for (TermCont::iterator it = sols->begin();
	 it != sols->end(); ++it)
      _solutions.top()->push_back(*it);

    delete sols;
  }

protected:
  void degenerisize() {
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
    ExternalTerm term(_dimension);
    while (!it.atEnd()) {
      it.getTerm(term);
      ++it;
      sols.push_back(term);
    }
  }

  void writeSolution(const ExternalTerm& b) {
    ASSERT(_dimension == b.getDimension());

    static vector<mpz_class> term(_dimension);
    for (unsigned int var = 0; var < _dimension; ++var)
      term[var] = (_translator->getExponent(var, b[var] + 1));
    _ioHandler->writeGeneratorOfIdeal(*_out, term, _names);
  }

  stack<TermCont*> _solutions;

  vector<bool> _firstPartition;

  ostream* _out;
  unsigned int _dimension;
  VarNames _names;
  bool _first;
  const TermTranslator* _translator;
  bool _doDegenerization;
  IOHandler* _ioHandler;
};

#endif
