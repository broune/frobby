#ifndef TERM_GUARD
#define TERM_GUARD

class ExternalTerm {
public:
  ExternalTerm(const ExternalTerm& term):
    _dimension(term._dimension) {
    _exponents = new Exponent[_dimension];
    copy(term._exponents,
	 term._exponents + _dimension,
	 _exponents);
  }

  ExternalTerm(unsigned int dimension):
    _dimension(dimension) {
    if (dimension > 0) {
      _exponents = new Exponent[_dimension];
      setToZero();
    } else
      _exponents = 0;
  }

  ExternalTerm():
    _exponents(0),
    _dimension(0) {
  }
    
  ~ExternalTerm() {
    delete[] _exponents;
  }

  const Exponent* begin(int position) const {
    ASSERT(position <= (int)_dimension);
    return _exponents + position;
  }

  const Exponent* end() const {
    return _exponents + _dimension;
  }

  unsigned int getDimension() const {
    return _dimension;
  }

  Exponent operator[](int position) const {
    return _exponents[position];
  }

  Exponent& operator[](int position) {
    return _exponents[position];
  }

  void setExponent(int position, Exponent value) const {
    _exponents[position] = value;
  }

  bool operator==(const ExternalTerm& term) const {
    for (unsigned int i = 0; i < getDimension(); ++i)
      if ((*this)[i] != term[i])
	return false;
    return true;
  }

  ExternalTerm& operator=(const ExternalTerm& term) {
    copy(term._exponents,
	 term._exponents + _dimension,
	 _exponents);

    return *this;
  }
  
  bool divides(const ExternalTerm& term) const {
    for (unsigned int i = 0; i < _dimension; ++i)
      if ((*this)[i] > term[i])
	return false;
    return true;
  }

  void lcm(const ExternalTerm& term1,
	   const ExternalTerm& term2, int position) {
    for (unsigned int i = position; i < _dimension; ++i) {
      Exponent e1 = term1[i];
      Exponent e2 = term2[i];
      setExponent(i, e1 > e2 ? e1 : e2);
    }
  }

  void gcd(const ExternalTerm& term1,
	   const ExternalTerm& term2, int position) {
    for (unsigned int i = position; i < _dimension; ++i) {
      Exponent e1 = term1[i];
      Exponent e2 = term2[i];
      setExponent(i, e1 < e2 ? e1 : e2);
    }
  }

  void resize(unsigned int dimension) {
    ASSERT(dimension > 0);

    delete[] _exponents;
    _dimension = dimension;
    if (dimension > 0)
      _exponents = new Exponent[dimension];
    else
      _exponents = 0;
  }

  bool operator<(const ExternalTerm& term) const {
    ASSERT(_dimension == term._dimension);
    for (unsigned int i = 0; i < _dimension; ++i) {
      if ((*this)[i] < term[i])
	return true;
      if ((*this)[i] > term[i])
	return false;
    }
    return false;
  }

  friend ostream& operator<<(ostream& out, const ExternalTerm& term) {
    out << '(' << term[0];
    for (unsigned int i = 1; i < term._dimension; ++i)
      out << ", " << term[i];
    out << ')';

    return out;
  }

  void setToZero() {
    fill_n(_exponents, _dimension, 0);
  }

private:
  Exponent* _exponents;
  unsigned int _dimension;
};  

#endif
