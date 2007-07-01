#ifndef STRATEGIES_GUARD
#define STRATEGIES_GUARD

#include "Partition.h"
#include "monosIO.h"
#include "Timer.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"

#include <stack>
#include <iomanip>
#include <iterator>

class Strategy {
public:
  virtual ~Strategy() {}

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) = 0;

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) = 0;

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) = 0;

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) = 0;

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    ASSERT(false);
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    ASSERT(false);
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
    string name;
    getName(name);
    cerr << "ERROR: doneDoingPartitionSet not implemented in " << name << "." << endl;
    ASSERT(false);
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    string name;
    getName(name);
    cerr << "ERROR: endingPartitioning not implemented in " << name << "." << endl;
    ASSERT(false);
  }

  virtual void getName(string& name) const = 0;
};

class PrintDebugStrategy : public Strategy {
public:
  PrintDebugStrategy() {}
  virtual ~PrintDebugStrategy() {
    cerr << "The computation is done." << endl;
  }

  virtual void getName(string& name) const {
    name = "PrintDebugStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    return false;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    if (tree.getPosition() == 0) {
      cerr << "Debug printing on. Starting the computation." << endl;
    }

    for (int i = 0; i < tree.getPosition(); ++i)
      cerr << '^';
    cerr << " Starting b=" << b
	 << " position=" << tree.getPosition() << endl;

    return false;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    for (int i = 0; i < tree.getPosition(); ++i)
      cerr << 'v';
    cerr << " Ending b=" << b
	 << " position=" << tree.getPosition() << endl;
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    for (unsigned int i = 0; i < b.getDimension(); ++i)
      cerr << '*';
    cerr << " Found solution " << b << endl;
  }

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    fill_n(ostream_iterator<char>(cerr), tree.getPosition(), '-');
    cerr << " starting partitioning" << endl;
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    cerr << "doing a set of the partition" << endl;
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
    cerr << "done doing that set of the partition" << endl;
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    cerr << "ending partitioning" << endl;
  }
};

class FrobeniusStrategy : public Strategy {
public:
  FrobeniusStrategy(const vector<mpz_class>& degrees,
		    mpz_class* frobeniusNumber,
		    unsigned int dimension,
		    const TermTranslator* translator,
		    bool useBound):
    _startingPartition(false),
    _storedMaximumDegreeSeen(dimension),
    _storedDegreeMultiples(dimension),
    _translator(translator),
    _dimension(dimension),
    _partialDegrees(dimension),
    _degreeMultiples(0),
    _initialDegree(degrees[0]),
    _maximumDegreeSeen(0),
    _frobeniusNumber(frobeniusNumber),
    _useBound(useBound) {
    ASSERT(degrees.size() == _dimension + 1);
    ASSERT(frobeniusNumber != 0);
    ASSERT(translator != 0);

    // If I code this using initialization syntax, the code output by
    // gcc v. 4.0.3 fools valgrind into thinking that the field has
    // not been set and that this is an error.
    _onlyTwoDimensional = (_dimension == 2);

    createDegreeMultiples(degrees);
  }

  virtual ~FrobeniusStrategy() {
    ASSERT(_degreeMultiples != 0);
    for (unsigned int i = 0; i < _dimension; ++i)
      delete[] _degreeMultiples[i];
    delete[] _degreeMultiples;

    *_frobeniusNumber = _maximumDegreeSeen;
  }

  virtual void getName(string& name) const {
    name = "FrobeniusStrategy";
  }


  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    return true;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    int position = tree.getPosition();

    if (!_startingPartition) {
      if (position == 0)
	_partialDegrees[0] = -_initialDegree;
      else {
	const Degree& degree = _degreeMultiples[position - 1][b[position - 1]];
	_partialDegrees[position] = _partialDegrees[position - 1] + degree;
      }
    } else {
      _partialDegrees[position] = 0;
      _startingPartition = false;
    }

    return !(_useBound &&
	     canSkipDueToUpperBound(tree, _partialDegrees[position]));
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    Degree degree;

    if (_startingPartition) {
      degree = _degreeMultiples[_dimension - 1][b[_dimension - 1]];
      _startingPartition = false;
    } else {
      degree =
	_degreeMultiples[_dimension - 2][b[_dimension - 2]] +
	_degreeMultiples[_dimension - 1][b[_dimension - 1]];
      if (!_onlyTwoDimensional) {
	degree += _partialDegrees[_dimension - 2];
      }
    }

    if (degree > _maximumDegreeSeen)
      _maximumDegreeSeen = degree;
  }

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    int position = tree.getPosition();

    _storedDegreeMultiples[position] = _degreeMultiples;
    _degreeMultiples = new Degree*[_dimension];

    _storedMaximumDegreeSeen[position] = _maximumDegreeSeen;
  }

  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    for (int i = _dimension - 1; i >= 0; --i) {
      if (compressor[i] == 0xFFFFFFFF)
	continue;
      _degreeMultiples[compressor[i]] = _storedDegreeMultiples[position][i];
    }
    _maximumDegreeSeen = 0;
    _startingPartition = true;

    _onlyTwoDimensional = (tree.getPosition() == (int)_dimension - 2);
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
    _partialDegrees[position] += _maximumDegreeSeen;
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    delete[] _degreeMultiples;
    _degreeMultiples = _storedDegreeMultiples[position];

    _maximumDegreeSeen = _storedMaximumDegreeSeen[position];
    if (_partialDegrees[position] > _maximumDegreeSeen)
      _maximumDegreeSeen = _partialDegrees[position];

    _onlyTwoDimensional = (_dimension == 2);
    _startingPartition = false;
  }

  void print(ostream& out) {
    out << "FrobeniusStrategy("
	<< "onlyTwoDimensional=" << _onlyTwoDimensional
	<< " startingPartition=" << _startingPartition
	<< " maximumDegreeSeen=" << _maximumDegreeSeen
	<< " partialDegrees=";
    copy(_partialDegrees.begin(),
	 _partialDegrees.end(),
	 ostream_iterator<Degree>(out, " "));
    out << endl;
  }
  
private:
  bool canSkipDueToUpperBound(const TermTree& tree, const Degree& degree) {
    unsigned int position = tree.getPosition();

    // Computing the bounds takes more time tham it saves in this
    // case.
    if (position > _dimension - 3)
      return false;

    ExternalTerm lcm(_dimension);
    tree.lcm(lcm);

    Degree upperBound = degree;
    for (unsigned int i = position; i < _dimension; ++i) {
      ASSERT(lcm[i] > 0);
      upperBound += _degreeMultiples[i][lcm[i] - 1];
    }

    return upperBound <= _maximumDegreeSeen;
  }


  void createDegreeMultiples(const vector<mpz_class>& degrees) {
    ASSERT(_degreeMultiples == 0);
    ASSERT(degrees.size() == _dimension + 1);

    _degreeMultiples = new Degree*[_dimension];
    for (unsigned int position = 0; position < _dimension; ++position) {
      Exponent range = _translator->getMaxId(position);

      _degreeMultiples[position] = new Degree[range];

      for (Exponent exponent = 0; exponent < range; ++exponent)
	_degreeMultiples[position][exponent] =
	  (_translator->getExponent(position, exponent + 1) - 1) *
	  degrees[position + 1];
    }
  }

  bool _startingPartition;
  vector<Degree> _storedMaximumDegreeSeen;
  vector<Degree**> _storedDegreeMultiples;
  bool _onlyTwoDimensional;
  const TermTranslator* _translator;
  unsigned int _dimension;
  vector<Degree> _partialDegrees;
  Degree** _degreeMultiples;
  Degree _initialDegree;
  Degree _maximumDegreeSeen;
  mpz_class* _frobeniusNumber;
  bool _useBound;
};

class BenchmarkStrategy : public Strategy {
public:
  BenchmarkStrategy() {}
  virtual ~BenchmarkStrategy() {}

  virtual void getName(string& name) const {
    name = "BenchmarkStrategy";
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
  }  

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
  }
};

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

class IntegerGapStrategy : public Strategy {
public:
  IntegerGapStrategy(Strategy* strategy,
		     unsigned int dimension,
		     const TermTranslator* translator):
    _strategy(strategy),
    _dimension(dimension),
    _minPowers(dimension),
    _maxPowers(dimension) {

    for (unsigned int i = 0; i < dimension; ++i) {
      _minPowers[i] = 1;
      if (translator->getExponent(i, _minPowers[i]) == 1)
	++_minPowers[i];

      _maxPowers[i] = translator->getMaxId(i);
      if (translator->getExponent(i, _maxPowers[i]) == 0)
	--_maxPowers[i];
    }
  }

  virtual ~IntegerGapStrategy() {
  }

  virtual void getName(string& name) const {
    name = "IntegerGapStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    return _strategy->consideringCall(b, sameExponentAsNext, tree);
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    for (unsigned int i = 0; i < _dimension; ++i)
      if (interesting(i, b[i] + 1))
	return _strategy->startingCall(b, tree, startingPartition);

    ExternalTerm term(_dimension);
    TermTree::TreeIterator it(tree);

    while (!it.atEnd()) {
      it.getTerm(term);
      for (unsigned int i = 0; i < _dimension; ++i)
	if (interesting(i, term[i]))
	  return _strategy->startingCall(b, tree, startingPartition);
      ++it;
    }

    return false;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    _strategy->endingCall(b, tree);
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    for (unsigned int i = 0; i < _dimension; ++i) {
      if (interesting(i, b[i] + 1)) {
	_strategy->foundSolution(b, startingPartition);
	return;
      }
    }
  }
  
protected:
  bool interesting(unsigned int position, Exponent exponent) const {
    return
      _minPowers[position] <= exponent &&
      exponent <= _maxPowers[position];
  }
    
  Strategy* _strategy;
  unsigned int _dimension;
  vector<Exponent> _minPowers;
  vector<Exponent> _maxPowers;
};

class StatisticsStrategy : public Strategy {
public:
  StatisticsStrategy(int dimension):
    _dimension(dimension),
    _callCounts(dimension) {
  }

  virtual ~StatisticsStrategy() {
    for (unsigned int position = 0; position < _dimension; ++position)
      cerr << "Level " << position + 1 << " had "
	   << _callCounts[position] << " calls. " << endl;
  }

  virtual void getName(string& name) const {
    name = "StatisticsStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    return false;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    return false;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    ++(_callCounts[tree.getPosition()]);
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    ++(_callCounts[_dimension - 1]);
  }

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
  }

private:
  unsigned int _dimension;
  vector<unsigned int> _callCounts;
};


class CompositeStrategy : public Strategy {
public:
  CompositeStrategy(Strategy* strategy1, Strategy* strategy2):
    _strategy1(strategy1),
    _strategy2(strategy2) {
    ASSERT(strategy1 != 0);
    ASSERT(strategy2 != 0);
  }

  virtual ~CompositeStrategy() {}

  virtual void getName(string& name) const {
    name = "CompositeStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    bool iSayContinue1 = _strategy1->consideringCall(b, sameExponentAsNext, tree);
    bool iSayContinue2 = _strategy2->consideringCall(b, sameExponentAsNext, tree);
    return iSayContinue1 || iSayContinue2;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    bool iSayContinue1 = _strategy1->startingCall(b, tree, startingPartition);
    bool iSayContinue2 = _strategy2->startingCall(b, tree, startingPartition);
    return iSayContinue1 || iSayContinue2;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    _strategy1->endingCall(b, tree);
    _strategy2->endingCall(b, tree);
  }
  
  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    _strategy1->foundSolution(b, startingPartition);
    _strategy2->foundSolution(b, startingPartition);
  }


  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    _strategy1->startingPartitioning(b, partition, tree);
    _strategy2->startingPartitioning(b, partition, tree);
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    _strategy1->doingPartitionSet(position, b, compressedB, partition, compressor, tree);
    _strategy2->doingPartitionSet(position, b, compressedB, partition, compressor, tree);
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
    _strategy1->doneDoingPartitionSet(position, compressor);
    _strategy2->doneDoingPartitionSet(position, compressor);
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    _strategy1->endingPartitioning(position, b);
    _strategy2->endingPartitioning(position, b);
  }

private:
  Strategy* _strategy1;
  Strategy* _strategy2;
};

class PrintProgressStrategy : public Strategy {
public:
  PrintProgressStrategy():
    _workTotal(0),
    _workDone(0) {}
  virtual ~PrintProgressStrategy() {}

  virtual void getName(string& name) const {
    name = "PrintProgressStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    ASSERT(_workTotal != 0);
    ASSERT(_workDone <= _workTotal);

    if (tree.getPosition() == 0) {
      if (_timeSinceLastReport.getSeconds() >= 5) {
	_timeSinceLastReport.reset();

	double doneRatio = ((double)_workDone)/_workTotal;
	cerr << setprecision(3)
	     << _workDone << '/' << _workTotal << '='
	     << doneRatio * 100.0 << "% done in " << _totalTime << '.' << endl;
      }

      ++_workDone;
    }

    return false;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    if (tree.getPosition() == 0) {
      _workTotal = 0;
      TermTree::TreeIterator treeIt(tree);
      while (!treeIt.atEnd()) {
	if (treeIt.getExponent(0) != 0)
	  ++_workTotal;
	++treeIt;
      }
    }

    return false;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
  }

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
  }

private:
  Timer _totalTime;
  Timer _timeSinceLastReport;
  unsigned int _workTotal;
  unsigned int _workDone;
};

class SkipRedundantStrategy : public Strategy {
public:
  SkipRedundantStrategy(Strategy* strategy,
			unsigned int dimension):
    _newSkip(dimension),
    _remember(dimension),
    _dimension(dimension),
    _strategy(strategy),
    _toAddNext(dimension),
    _seenHasChanged(dimension) {
    for (unsigned int position = 0; position < _dimension; ++position)
      _skip.push_back(TermTree(dimension, position));
  }

  virtual ~SkipRedundantStrategy() {}

  virtual void getName(string& name) const {
    name = "SkipRedundantStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    unsigned int position = tree.getPosition();

    _remember[position] = sameExponentAsNext;

    //cout << "looking at possible skip for " << position << ' ' << b << endl;
    //print(cout);

    if (_skip[position].getDivisor(b) != 0) {
      if (!sameExponentAsNext)
	_newSkip[position + 1].clear();
      //cout << "skipped" << endl;
      return false;
    }

    return _strategy->consideringCall(b, sameExponentAsNext, tree);
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    unsigned int position = tree.getPosition();

    if (!startingPartition) {
      _skip[position].clear();
      _skip[position].setThreshold(b);
      if (position > 0)
	_skip[position].insertProjectionOf(_skip[position - 1], 0, b[position - 1]);
      for (unsigned int i = 0; i < _newSkip[position].size(); ++i)
	_skip[position].insert(_newSkip[position][i]);
    }

    _remember[position] = false;
      
    return _strategy->startingCall(b, tree, startingPartition);
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    unsigned int position = tree.getPosition();

    if (position > 0) {
      if (_remember[position - 1])
	_newSkip[position].push_back(b);
      else
	_newSkip[position].clear();
    }

    _strategy->endingCall(b, tree);
  }
  
  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    //cout << "sol " << b << ' ' << startingPartition << endl;
    //print(cout);
    if ((startingPartition && _skip[_dimension - 1].getDivisor(b) == 0) ||
	(!startingPartition && _skip[_dimension - 2].getDivisor(b) == 0))
      _strategy->foundSolution(b, startingPartition);
    //else
    //  cout << "skipped sol" << endl;
  }


  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    unsigned int position = tree.getPosition();

    _skip[position].clear();
    _skip[position].setThreshold(b);
    if (position > 0)
      _skip[position].insertProjectionOf(_skip[position - 1], 0, b[position - 1]);
    for (unsigned int i = 0; i < _newSkip[position].size(); ++i)
      _skip[position].insert(_newSkip[position][i]);

    _remember[position] = false; // TODO: is this necessary?

    _strategy->startingPartitioning(b, partition, tree);
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    int nextPosition = tree.getPosition();
    TermTree::TreeIterator it((TermTree&)_skip[position]);

    _skip[nextPosition].clear();
    _skip[nextPosition].setThreshold(compressedB);

    //cout << "compressing _skip. b=" << b << ", nextPosition=" << nextPosition << endl;
    ExternalTerm term(_dimension);
    while (!it.atEnd()) {
      it.getTerm(term);

      //cout << "compressing: " << term << endl;

      for (int i = _dimension - 1; i >= position; --i) {
	if (compressor[i] == 0xFFFFFFFF) {
	  if (term[i] > b[i])
	    goto skip;
	} else
	  term[compressor[i]] = term[i];
      }
      _skip[nextPosition].insert(term);

    skip:
      ++it;
    }

    _strategy->doingPartitionSet(position, b, compressedB, partition, compressor, tree);
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
    _strategy->doneDoingPartitionSet(position, compressor);
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    if (position > 0) {
      if (_remember[position - 1])
	_newSkip[position].push_back(b);
      else
	_newSkip[position].clear();
    }

    _strategy->endingPartitioning(position, b);
  }

  void print(ostream& out) {
    // If there is a segmentation violation below, the flush will make
    // it clear that this is where it happened.
    out << "SkipRedundantStrategy(dimension=" << flush;
    out << _dimension << '\n';
    out << " remember=";
    copy(_remember.begin(), _remember.end(),
	 ostream_iterator<bool>(out));
    out << '\n';

    out << " skip=\n";
    for (unsigned int i = 0; i < _dimension; ++i) {
      out << "  " << i << ": ";
      _skip[i].print(out);
    }

    out << " newSkip=\n";
    for (unsigned int i = 0; i < _dimension; ++i) {
      out << "  " << i << ": ";
      copy(_newSkip[i].begin(), _newSkip[i].end(),
	   ostream_iterator<ExternalTerm>(out));
      out << '\n';
    }

    out << ")" << endl;
  }

private:
  vector<TermTree> _skip;
  vector<vector<ExternalTerm> > _newSkip;
  vector<bool> _remember;
  unsigned int _dimension;
  Strategy* _strategy;

  vector<Exponent> _toAddNext;
  vector<TermTree> _seen;
  vector<TermTree> _projectedSeen;
  vector<bool> _seenHasChanged;
};

#endif
