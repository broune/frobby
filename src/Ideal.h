/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#ifndef IDEAL_GUARD
#define IDEAL_GUARD

#include <vector>
#include <algorithm>

class Ideal {
  typedef vector<Exponent*> Cont;

 public:
  Ideal(size_t varCount = 0);
  Ideal(const Ideal& ideal);
  ~Ideal();

  // *** Accessors

  typedef Cont::const_iterator const_iterator;
  typedef Cont::iterator iterator;

  const_iterator begin() const {return _terms.begin();}
  const_iterator end() const {return _terms.end();}
  Exponent* operator[](size_t index) {return _terms[index];}

  iterator begin() {return _terms.begin();}
  iterator end() {return _terms.end();}
  const Exponent* operator[](size_t index) const {return _terms[index];}  

  size_t getVarCount() const {return _varCount;}
  size_t getGeneratorCount() const {return _terms.size();}

  bool isIncomparable(const Exponent* term) const;

  // Returns true if any generator divides term.
  bool contains(const Exponent* term) const;

  // Returns true if the identity is one of the generators.
  bool containsIdentity() const;

  // Returns true if some minimal generator strictly divides term.
  bool strictlyContains(const Exponent* term) const;

  // Returns true if no generator divides another.
  bool isMinimallyGenerated() const;

  // Returns true if there are no generators.
  bool isZeroIdeal() const;

  // Returns true if all generators are pure powers. This only
  // corresponds to the mathematical definition of an irreducible
  // polynomial ideal if the ideal is minimally generated.
  bool isIrreducible() const;

  // Returns true if no non-zero exponent of the same variable appears
  // in two distinct generators. This only corresponds to the
  // mathematical definition of strongly generic if the ideal is
  // minimally generated. This method is not const because it permutes
  // the generators.
  bool isStronglyGeneric();

  void getLcm(Exponent* lcm) const;
  void getGcd(Exponent* gcd) const;

  // least[var] will be the smallest non-zero exponent of var that
  // appears among the generators.
  void getLeastExponents(Exponent* least) const;

  // counts[var] will be the number of generators divisible by var.
  void getSupportCounts(Exponent* counts) const;

  // returns the first generator that var divides or end() if no such
  // generator exists.
  const_iterator getMultiple(size_t var) const;

  bool operator==(const Ideal& ideal) const;

  void print(FILE* file) const;

  // *** Mutators

  // Insert generators into the ideal.
  void insert(const Exponent* term);
  void insert(const Ideal& ideal);

  // This is equivalent to calling insert and then minimize.
  void insertReminimize(const Exponent* term);

  // Remove non-redundant generators.
  void minimize();

  // Sorts the generators in the specified order
  void sortReverseLex(); // reverse lexicographic order
  void sortLex(); // lexicographic order

  // Sort the generators in ascending order according to the exponent of var.
  void singleDegreeSort(size_t var);

  // Replace each generator g by g * term.
  void product(const Exponent* term);

  // Replace each generator g by g : by. The second overload has by
  // equal to var raised to e.
  void colon(const Exponent* by);
  void colon(size_t var, Exponent e);

  // Equivalent to calling colon(by) and then minimize. The second
  // overload has by equal to var raised to e. Returns true if the support
  // of any generator was changed.
  bool colonReminimize(const Exponent* colon);
  bool colonReminimize(size_t var, Exponent e);

  // Swaps it and the last element, and then removes the last element,
  // which is the element originally pointed to by it.
  void remove(const_iterator it);

  // Removes those generators that are multiples of term. The second
  // overload has term equal to var raised to e.
  void removeMultiples(const Exponent* term);
  void removeMultiples(size_t var, Exponent e);

  // Insert those generators of ideal that are not multiples of
  // term. The second overload has term equal to var raised to e.
  void insertNonMultiples(const Exponent* term, const Ideal& ideal);
  void insertNonMultiples(size_t var, Exponent e, const Ideal& ideal);

  // Removes those generators that are strict multiples of term.
  void removeStrictMultiples(const Exponent* term);

  // Remove duplicate generators.
  void removeDuplicates();

  // Removes all generators, and optionally sets the number of variables.
  void clear();
  void clearAndSetVarCount(size_t varCount);

  Ideal& operator=(const Ideal& ideal);

  void swap(Ideal& ideal);

  // Removes those generators m such that pred(m) evaluates to
  // true. Returns true if any generators were removed.
  template<class Predicate>
    bool removeIf(Predicate pred);

  // Ideal caches memory allocated with new internally and reuses it
  // to avoid calling new all the time. Call this method to release
  // the cache.
  static void clearStaticCache();

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
