#ifndef TERM_LIST_GUARD
#define TERM_LIST_GUARD

#include "Ideal.h"

typedef Ideal TermList;


/*
#include "Term.h"
#include "Ideal.h"

class TermList : public Ideal {
  typedef vector<Exponent*> Cont;

 public:
  typedef Cont::iterator iterator;
  typedef Cont::const_iterator const_iterator;

  TermList(unsigned int varCount);
  TermList(const Ideal& ideal);
  TermList(const TermList& ideal);

  virtual bool filter(FilterFunction& function);

  void insert(const Term& term);
  void insert(const Exponent* exponents);
  void insert(const Ideal& ideal);

  void singleDegreeSort(size_t variable);


  void minimize();
  void colon(const Term& by);
  void colonReminimize(size_t var, Exponent exp);
  void colonReminimize(const Term& by);

  Ideal* createMinimizedColon(const Term& by) const;
  Ideal* clone() const;
  Ideal* createNew(size_t varCount);
  void clear();

  bool removeStrictMultiples(const Exponent* term);

  void print() const;

  static void* operator new(size_t size);
  void operator delete(void* p, size_t size);

  virtual void removeDuplicates();
};
*/
#endif
