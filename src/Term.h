#ifndef TERM_GUARD
#define TERM_GUARD

// TODO: rename Term to Term

class Term {
 public:
  Term(const Term& term);
  Term(unsigned int dimension);
  Term();
  ~Term();

  const Exponent* begin(int position) const;

  const Exponent* end() const;

  unsigned int getDimension() const;

  Exponent operator[](int position) const;

  Exponent& operator[](int position);

  void setExponent(int position, Exponent value) const;

  bool operator==(const Term& term) const;

  Term& operator=(const Term& term);
  
  bool divides(const Term& term) const;
  bool strictlyDivides(const Term& term) const;

  void lcm(const Term& term1,
	   const Term& term2, int position = 0);

  void gcd(const Term& term1,
	   const Term& term2, int position = 0);

  void resize(unsigned int dimension);

  bool operator<(const Term& term) const;

  friend ostream& operator<<(ostream& out, const Term& term) {
    out << '(' << term[0];
    for (unsigned int i = 1; i < term._dimension; ++i)
      out << ", " << term[i];
    out << ')';
  
    return out;
  }

  void product(const Term& a, const Term& b);

  void setToZero(); // TODO: rename to setToOne
  bool isZero() const; // TODO: rename to isOne

  // this = a : b, which is lcm(a,b)/b
  void colon(const Term& a, const Term& b);

 private:
  Exponent* _exponents;
  unsigned int _dimension;
};  

#endif
