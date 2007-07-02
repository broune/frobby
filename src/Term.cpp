#include "stdinc.h"
#include "Term.h"

Term::Term(const Term& term):
  _dimension(term._dimension) {
  _exponents = new Exponent[_dimension];
  copy(term._exponents,
       term._exponents + _dimension,
       _exponents);
}

Term::Term(unsigned int dimension):
  _dimension(dimension) {
  if (dimension > 0) {
    _exponents = new Exponent[_dimension];
    setToZero();
  } else
    _exponents = 0;
}

Term::Term():
  _exponents(0),
  _dimension(0) {
}
    
Term::~Term() {
  delete[] _exponents;
}

const Exponent* Term::begin(int position) const {
  ASSERT(position <= (int)_dimension);
  return _exponents + position;
}

const Exponent* Term::end() const {
  return _exponents + _dimension;
}

unsigned int Term::getDimension() const {
  return _dimension;
}

Exponent Term::operator[](int position) const {
  return _exponents[position];
}

Exponent& Term::operator[](int position) {
  return _exponents[position];
}

void Term::setExponent(int position, Exponent value) const {
  _exponents[position] = value;
}

bool Term::operator==(const Term& term) const {
  for (unsigned int i = 0; i < getDimension(); ++i)
    if ((*this)[i] != term[i])
      return false;
  return true;
}

Term& Term::operator=(const Term& term) {
  copy(term._exponents,
       term._exponents + _dimension,
       _exponents);
  
  return *this;
}

bool Term::divides(const Term& term) const {
  for (unsigned int i = 0; i < _dimension; ++i)
    if ((*this)[i] > term[i])
      return false;
  return true;
}

void Term::lcm(const Term& term1,
		       const Term& term2, int position) {
  for (unsigned int i = position; i < _dimension; ++i) {
    Exponent e1 = term1[i];
    Exponent e2 = term2[i];
    setExponent(i, e1 > e2 ? e1 : e2);
  }
}

void Term::gcd(const Term& term1,
		       const Term& term2, int position) {
  for (unsigned int i = position; i < _dimension; ++i) {
    Exponent e1 = term1[i];
    Exponent e2 = term2[i];
    setExponent(i, e1 < e2 ? e1 : e2);
  }
}

void Term::resize(unsigned int dimension) {
  ASSERT(dimension > 0);
  
  delete[] _exponents;
  _dimension = dimension;
  if (dimension > 0)
    _exponents = new Exponent[dimension];
  else
    _exponents = 0;
}

bool Term::operator<(const Term& term) const {
  ASSERT(_dimension == term._dimension);
  for (unsigned int i = 0; i < _dimension; ++i) {
    if ((*this)[i] < term[i])
      return true;
    if ((*this)[i] > term[i])
	return false;
  }
  return false;
}

void Term::setToZero() {
  fill_n(_exponents, _dimension, 0);
}
