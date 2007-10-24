#ifndef TERM_LIST_GUARD
#define TERM_LIST_GUARD

#include "Term.h"
#include "Ideal.h"

class ExponentAllocator {
 public:
  ExponentAllocator(size_t varCount):
    _varCount(varCount),
    _chunkSize(varCount * 100),
    _chunk(0),
    _chunkIterator(0),
    _chunkEnd(0) {
    ASSERT(varCount >= 1);
  }

  ~ExponentAllocator() {
    clear();
  }

  Exponent* allocate() {
    if (_chunkIterator == _chunkEnd) {
      if (_chunk != 0)
	_chunks.push_back(_chunk);
      _chunk = new Exponent[_chunkSize];
      _chunkIterator = _chunk;
      _chunkEnd = _chunk + _chunkSize;
    }

    Exponent* term = _chunkIterator;
    _chunkIterator += _varCount;
    return term;
  }

  void clear() {
    delete[] _chunk;
    _chunk = 0;
    _chunkIterator = 0;
    _chunkEnd = 0;

    for (size_t i = 0; i < _chunks.size(); ++i)
      delete[] _chunks[i];
    _chunks.clear();
  }

 private:
  size_t _varCount;
  size_t _chunkSize;

  Exponent* _chunk;
  Exponent* _chunkIterator;
  Exponent* _chunkEnd;

  vector<Exponent*> _chunks;
};

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

  const_iterator begin() const;
  const_iterator end() const;

  void singleDegreeSort(size_t variable);

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

  bool removeStrictMultiples(const Term& term);

  void print() const;

private:
  size_t _varCount;
  vector<Exponent*> _terms;
  ExponentAllocator _allocator;
};

#endif
