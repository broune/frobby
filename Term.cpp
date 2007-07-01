#include "stdinc.h"
#include "Term.h"

ExternalTerm::ExternalTerm(const ExternalTerm& term):
  _dimension(term._dimension) {
  _exponents = new Exponent[_dimension];
  copy(term._exponents,
       term._exponents + _dimension,
       _exponents);
}

ExternalTerm::ExternalTerm(unsigned int dimension):
  _dimension(dimension) {
  if (dimension > 0) {
    _exponents = new Exponent[_dimension];
    setToZero();
  } else
    _exponents = 0;
}

ExternalTerm::ExternalTerm():
  _exponents(0),
  _dimension(0) {
}
    
ExternalTerm::~ExternalTerm() {
  delete[] _exponents;
}

const Exponent* ExternalTerm::begin(int position) const {
  ASSERT(position <= (int)_dimension);
  return _exponents + position;
}

const Exponent* ExternalTerm::end() const {
  return _exponents + _dimension;
}

unsigned int ExternalTerm::getDimension() const {
  return _dimension;
}

Exponent ExternalTerm::operator[](int position) const {
  return _exponents[position];
}

Exponent& ExternalTerm::operator[](int position) {
  return _exponents[position];
}

void ExternalTerm::setExponent(int position, Exponent value) const {
  _exponents[position] = value;
}

bool ExternalTerm::operator==(const ExternalTerm& term) const {
  for (unsigned int i = 0; i < getDimension(); ++i)
    if ((*this)[i] != term[i])
      return false;
  return true;
}

ExternalTerm& ExternalTerm::operator=(const ExternalTerm& term) {
  copy(term._exponents,
       term._exponents + _dimension,
       _exponents);
  
  return *this;
}

bool ExternalTerm::divides(const ExternalTerm& term) const {
  for (unsigned int i = 0; i < _dimension; ++i)
    if ((*this)[i] > term[i])
      return false;
  return true;
}

void ExternalTerm::lcm(const ExternalTerm& term1,
		       const ExternalTerm& term2, int position) {
  for (unsigned int i = position; i < _dimension; ++i) {
    Exponent e1 = term1[i];
    Exponent e2 = term2[i];
    setExponent(i, e1 > e2 ? e1 : e2);
  }
}

void ExternalTerm::gcd(const ExternalTerm& term1,
		       const ExternalTerm& term2, int position) {
  for (unsigned int i = position; i < _dimension; ++i) {
    Exponent e1 = term1[i];
    Exponent e2 = term2[i];
    setExponent(i, e1 < e2 ? e1 : e2);
  }
}

void ExternalTerm::resize(unsigned int dimension) {
  ASSERT(dimension > 0);
  
  delete[] _exponents;
  _dimension = dimension;
  if (dimension > 0)
    _exponents = new Exponent[dimension];
  else
    _exponents = 0;
}

bool ExternalTerm::operator<(const ExternalTerm& term) const {
  ASSERT(_dimension == term._dimension);
  for (unsigned int i = 0; i < _dimension; ++i) {
    if ((*this)[i] < term[i])
      return true;
    if ((*this)[i] > term[i])
	return false;
  }
  return false;
}

void ExternalTerm::setToZero() {
  fill_n(_exponents, _dimension, 0);
}
