#ifndef TERM_GUARD
#define TERM_GUARD

// TODO: rename ExternalTerm to Term

class ExternalTerm {
 public:
  ExternalTerm(const ExternalTerm& term);
  ExternalTerm(unsigned int dimension);
  ExternalTerm();
  ~ExternalTerm();

  const Exponent* begin(int position) const;

  const Exponent* end() const;

  unsigned int getDimension() const;

  Exponent operator[](int position) const;

  Exponent& operator[](int position);

  void setExponent(int position, Exponent value) const;

  bool operator==(const ExternalTerm& term) const;

  ExternalTerm& operator=(const ExternalTerm& term);
  
  bool divides(const ExternalTerm& term) const;

  void lcm(const ExternalTerm& term1,
	   const ExternalTerm& term2, int position);

  void gcd(const ExternalTerm& term1,
	   const ExternalTerm& term2, int position);

  void resize(unsigned int dimension);

  bool operator<(const ExternalTerm& term) const;

  friend ostream& operator<<(ostream& out, const ExternalTerm& term) {
    out << '(' << term[0];
    for (unsigned int i = 1; i < term._dimension; ++i)
      out << ", " << term[i];
    out << ')';
  
    return out;
  }

  void setToZero();

 private:
  Exponent* _exponents;
  unsigned int _dimension;
};  

#endif
