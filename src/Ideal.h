#ifndef IDEAL_GUARD
#define IDEAL_GUARD

#include "Term.h"

class Ideal {
  typedef vector<Exponent*> Cont;
  typedef Cont::iterator iterator;

public:
  Ideal(unsigned int varCount = 0);
  Ideal(const Ideal& ideal);
  ~Ideal();

  // *** Accessors

  typedef Cont::const_iterator const_iterator;

  const_iterator begin() const {return _terms.begin();}
  const_iterator end() const {return _terms.end();}
  size_t getVarCount() const {return _varCount;}
  size_t getGeneratorCount() const {return _terms.size();}

  bool isIncomparable(const Term& term) const;
  bool isIncomparable(const Exponent* term) const;

  bool contains(const Term& term) const;
  bool contains(const Exponent* term) const;

  bool isIrreducible() const;

  void getLcm(Term& lcm) const;
  void getGcd(Term& gcd) const;

  void print(ostream& out) const;
  friend ostream& operator<<(ostream& out, const Ideal& ideal) {
    ideal.print(out);
    return out;
  }

  // *** Mutators

  void insert(const Term& term);
  void insert(const Exponent* term);
  void insert(const Ideal& term);

  // Remove non-redundant generators.
  void minimize();

  // Replace each generator g by g : by.
  void colon(const Term& by);

  // Equivalent to calling colon(by) and then minimize.
  void colonReminimize(const Term& by);

  // Perform colon by var raised to exp and then minimize.
  void colonReminimize(size_t var, Exponent exp);

  // a is a strict multiple of b if b[i] > 0 implies that a[i] > b[i].
  void removeStrictMultiples(const Exponent* exponent);

  // Remove duplicate generators.
  void removeDuplicates();

  // Sort the generators in ascending order according to the exponent of var.
  void singleDegreeSort(size_t var);

  // Removes all generators, and optionally sets the number of variables.
  void clear();
  void clearAndSetVarCount(size_t varCount);


  Ideal& operator=(const Ideal& ideal);

  void swap(Ideal& ideal);

  // Removes those generators m such that pred(m) evaluates to
  // true. Returns true if any generators were removed.
  template<class Predicate>
    bool removeIf(Predicate pred);
  
 protected:
  class ExponentAllocator {
  public:
    ExponentAllocator(size_t varCount);
    ~ExponentAllocator();

    Exponent* allocate();
    void reset(size_t newVarCount);

    void swap(ExponentAllocator& allocator);

  private:
    ExponentAllocator(const ExponentAllocator&);
    ExponentAllocator& operator=(const ExponentAllocator&);

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

template<class Predicate>
inline bool Ideal::removeIf(Predicate pred) {
  iterator newEnd = std::remove_if(_terms.begin(), _terms.end(), pred);

  if (newEnd != _terms.end()) {
    _terms.erase(newEnd, _terms.end());
    return true;
  } else
    return false;
}

#endif
