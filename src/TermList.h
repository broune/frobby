#ifndef TERM_LIST_GUARD
#define TERM_LIST_GUARD

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

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  void singleDegreeSort(size_t variable);

  bool isIncomparable(const Term& term) const;

  size_t getVarCount() const;
  size_t getGeneratorCount() const;
  bool isZeroIdeal() const;

  void getLcm(Term& lcm) const;
  void getGcd(Term& gcd) const;

  bool contains(const Exponent* term) const;

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
 
 private:
  class ExponentAllocator {
  public:
    ExponentAllocator(size_t varCount);
    ~ExponentAllocator();

    Exponent* allocate();
    void reset(size_t newVarCount = 0);

  private:
    bool useSingleChunking() const;

    size_t _varCount;

    Exponent* _chunk;
    Exponent* _chunkIterator;
    Exponent* _chunkEnd;

    vector<Exponent*> _chunks;
  };

  size_t _varCount;
  vector<Exponent*> _terms;
  ExponentAllocator _allocator;
};

#endif
