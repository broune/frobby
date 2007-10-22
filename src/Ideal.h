#ifndef IDEAL_GUARD
#define IDEAL_GUARD

#include "Term.h"

class Ideal {
  typedef vector<Term> Cont;

public:
  typedef Cont::iterator iterator;
  typedef Cont::const_iterator const_iterator;

  virtual ~Ideal();

  virtual void insert(const Term& term) = 0;

  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;

  virtual bool isIncomparable(const Term& term) const = 0;

  virtual size_t getVariableCount() const = 0;
  virtual size_t getGeneratorCount() const = 0;
  virtual bool isZeroIdeal() const = 0;

  virtual void getLcm(Term& lcm) const = 0;
  virtual void getGcd(Term& gcd) const = 0;

  virtual bool contains(const Term& term) const = 0;

  virtual void minimize() = 0;
  virtual void colon(const Term& by) = 0;

  virtual Ideal* createMinimizedColon(const Term& by) const = 0;
  virtual Ideal* clone() const = 0;
  virtual void clear() = 0;

  virtual void removeStrictMultiples(const Term& term) = 0;

  virtual void print() const = 0;
};

#endif
