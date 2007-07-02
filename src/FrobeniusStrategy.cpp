#include "stdinc.h"
#include "FrobeniusStrategy.h"

#include "Strategy.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "Partition.h"

FrobeniusStrategy::FrobeniusStrategy(const vector<mpz_class>& degrees,
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

FrobeniusStrategy::~FrobeniusStrategy() {
  ASSERT(_degreeMultiples != 0);
  for (unsigned int i = 0; i < _dimension; ++i)
    delete[] _degreeMultiples[i];
  delete[] _degreeMultiples;
  
  *_frobeniusNumber = _maximumDegreeSeen - _initialDegree;
}

void FrobeniusStrategy::getName(string& name) const {
  name = "FrobeniusStrategy";
}

bool FrobeniusStrategy::consideringCall(const Term& b,
					bool sameExponentAsNext,
					const TermTree& tree) {
  return true;
}

bool FrobeniusStrategy::startingCall(const Term& b,
				     const TermTree& tree,
				     bool startingPartition) {
  int position = tree.getPosition();
  
  if (!_startingPartition) {
    if (position == 0)
      _partialDegrees[0] = 0;
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

void FrobeniusStrategy::endingCall(const Term& b,
				   const TermTree& tree) {
}

void FrobeniusStrategy::foundSolution(const Term& b,
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

void FrobeniusStrategy::startingPartitioning(const Term& b,
					     const Partition& partition,
					     const TermTree& tree) {
  int position = tree.getPosition();
  
  _storedDegreeMultiples[position] = _degreeMultiples;
  _degreeMultiples = new Degree*[_dimension];

  _storedMaximumDegreeSeen[position] = _maximumDegreeSeen;
}

void FrobeniusStrategy::doingPartitionSet(int position,
					  const Term& b,
					  const Term& compressedB,
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

void FrobeniusStrategy::doneDoingPartitionSet
(int position,
 const vector<Exponent>& compressor) {
  _partialDegrees[position] += _maximumDegreeSeen;
}

void FrobeniusStrategy::endingPartitioning(int position,
					   const Term& b) {
  delete[] _degreeMultiples;
  _degreeMultiples = _storedDegreeMultiples[position];

  _maximumDegreeSeen = _storedMaximumDegreeSeen[position];
  if (_partialDegrees[position] > _maximumDegreeSeen)
    _maximumDegreeSeen = _partialDegrees[position];

  _onlyTwoDimensional = (_dimension == 2);
  _startingPartition = false;
}

void FrobeniusStrategy::print(ostream& out) {
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

bool FrobeniusStrategy::
canSkipDueToUpperBound(const TermTree& tree, const Degree& degree) {
  unsigned int position = tree.getPosition();
  
  // Computing the bounds takes more time tham it saves in this
  // case.
  if (position > _dimension - 3)
    return false;

  Term lcm(_dimension);
  tree.lcm(lcm);

  Degree upperBound = degree;
  for (unsigned int i = position; i < _dimension; ++i) {
    ASSERT(lcm[i] > 0);
    upperBound += _degreeMultiples[i][lcm[i] - 1];
  }

  return upperBound <= _maximumDegreeSeen;
}

void FrobeniusStrategy::
createDegreeMultiples(const vector<mpz_class>& degrees) {
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
