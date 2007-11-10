#ifndef IDEAL_GUARD
#define IDEAL_GUARD

#include "Term.h"

class Ideal {
  typedef vector<Exponent*> Cont;

public:
  Ideal(unsigned int varCount);
  Ideal(const Ideal& ideal);

  typedef Cont::iterator iterator;
  typedef Cont::const_iterator const_iterator;

  iterator begin() {return _terms.begin();}
  iterator end() {return _terms.end();}
  const_iterator begin() const {return _terms.begin();}
  const_iterator end() const {return _terms.end();}
  size_t getVarCount() const {return _varCount;}
  size_t getGeneratorCount() const {return _terms.size();}

  bool isIncomparable(const Term& term) const;

  bool isZeroIdeal() const;

  void getLcm(Term& lcm) const;
  void getGcd(Term& gcd) const;

  bool contains(const Exponent* term) const;


  class FilterFunction {
  public:
    virtual ~FilterFunction() {}
    virtual bool operator()(const Exponent* term) = 0;
  };

  // Returns true if any terms were removed.
  virtual bool filter(FilterFunction& function);

  virtual ~Ideal();

  virtual void insert(const Exponent* term);
  virtual void insert(const Term& term);
  virtual void insert(const Ideal& term);


  virtual void singleDegreeSort(size_t variable);


  virtual void minimize();
  virtual void colon(const Term& by);
  virtual void colonReminimize(const Term& by);
  virtual void colonReminimize(size_t var, Exponent exp);

  virtual Ideal* createMinimizedColon(const Term& by) const;
  virtual Ideal* clone() const;
  virtual Ideal* createNew(size_t varCount) const;
  virtual void clear();

  // Returns true if any were removed.
  virtual bool removeStrictMultiples(const Exponent* exponent);

  virtual void print() const;

  bool isIrreducible() const;

  virtual void removeDuplicates();
  /*
  static void* operator new(size_t size);
  void operator delete(void* p, size_t size);
  */

 protected:
  class ExponentAllocator {
  public:
    ExponentAllocator(size_t varCount);
    ~ExponentAllocator();

    Exponent* allocate();
    void reset(size_t newVarCount);

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

#endif
