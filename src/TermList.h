#ifndef TERM_LIST_GUARD
#define TERM_LIST_GUARD

#include "Term.h"
#include "Ideal.h"

class TermList : public Ideal {
  typedef vector<Term> Cont;

public:
  typedef Cont::iterator iterator;
  typedef Cont::const_iterator const_iterator;

  TermList(unsigned int varCount);
  TermList(const Ideal& ideal);

  void insert(const Term& term);

  const_iterator begin() const;

  const_iterator end() const;

  bool isIncomparable(const Term& term) const;

  size_t size() const; // TODO: remove this

  size_t getVariableCount() const;
  size_t getGeneratorCount() const;
  bool isZeroIdeal() const;

  void getLcm(Term& lcm) const;
  void getGcd(Term& gcd) const;

  bool contains(const Term& term) const;

  void minimize();
  void colon(const Term& by);

  Ideal* createMinimizedColon(const Term& by) const;
  Ideal* clone() const;
  void clear();

  void removeStrictMultiples(const Term& term);

  void print() const;

private:
  unsigned int _varCount;
  Cont _terms;
};

#endif
