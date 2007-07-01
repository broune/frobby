#ifndef FROBENIUS_ACTION_GUARD
#define FROBENIUS_ACTION_GUARD

#include "Strategy.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"
#include "Partition.h"

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

#endif
