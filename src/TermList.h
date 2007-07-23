#ifndef TERM_LIST_GUARD
#define TERM_LIST_GUARD

#include "Term.h"

class TermList {
  typedef vector<Term> Cont;

public:
  typedef Cont::iterator iterator;
  typedef Cont::const_iterator const_iterator;

  TermList(unsigned int varCount);

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

  void colonMinimize(const TermList& ideal, const Term& by);
  void colon(const Term& by);

  void minimize();

  void removeStrictMultiples(const Term& term);

  void print() const;

private:
  Cont _terms;
  unsigned int _varCount;
};

#endif
