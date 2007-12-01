#ifndef SLICE_GUARD
#define SLICE_GUARD

#include "Term.h"
#include "Ideal.h"

class DecomConsumer;

class Slice {
 public:
  Slice();
  Slice(const Ideal& ideal, const Ideal& subtract, const Term& multiply);

  // *** Accessors

  size_t getVarCount() const {return _varCount;}

  const Ideal& getIdeal() const {return _ideal;}

  Ideal& getSubtract() {return _subtract;}
  const Ideal& getSubtract() const {return _subtract;}

  Term& getMultiply() {return _multiply;}
  const Term& getMultiply() const {return _multiply;}

  // Returns the least common multiple of the generators of
  // getIdeal().
  const Term& getLcm() const;

  void print(ostream& out) const;
  friend ostream& operator<<(ostream& out, const Slice& slice) {
    slice.print(out);
    return out;
  }

  // *** Mutators

  // Removes all generators of getIdeal() and getSubtract() and sets
  // getMultiply() to the identity. Also changes getVarCount() to varCount.
  void resetAndSetVarCount(size_t varCount);

  // Clears getIdeal() and getSubtract(). Does not change getMultiply().
  void clear();

  // Calls singleDegreeSort on getIdeal().
  void singleDegreeSortIdeal(size_t var);

  // Inserts term into getIdeal().
  void insertIntoIdeal(const Exponent* term);

  // Efficiently swaps the values of *this and slice while avoiding
  // copies.
  void swap(Slice& slice);

  // Computes an inner slice with the specified pivot, i.e. a colon by
  // pivot is applied to getMultiply() and getSubtract(), while
  // getMultiply() is multiplied by pivot. 
  void innerSlice(const Term& pivot);

  // Computes an outer slice with the specified pivot, i.e. strict
  // multiples of pivot are removed from getIdeal(), and pivot is
  // added to getSubtract() if necessary.
  void outerSlice(const Term& pivot);

  // Returns true if a base case is reached, and in that case outputs
  // the content to consumer. The slice must be fully simplified.
  //
  // A base case is reached if not all variables divide the lcm of
  // getIdeal(), or if getGeneratorCount() is 2, or if getIdeal() is
  // square free.
  bool baseCase(DecomConsumer* consumer);

  // Simplies the slice such that normalize, pruneSubtract,
  // removeDoubleLcm and applyLowerBound all return false. simplify
  // works regardless, but it is optimized for the case where the
  // slice is already normalized (an ASSERT catches if this is not
  // so).
  void simplify();

  // Removes those generators of getIdeal() that are strict multiples
  // of some generator of getSubtract(). Returns true if any
  // generators were removed.
  bool normalize();

  // Removes those generators of subtract that do not strictly divide
  // the lcm of getIdeal(), or that lies within the ideal
  // getIdeal(). Returns true if any generators were removed.
  bool pruneSubtract();

  // Removes those generators g of getIdeal() such that g[i] equals
  // getLcm()[i] for two distinct i. This is done iteratively until no
  // more generators can be removed in this way. Returns true if any
  // generators were removed or a base case has been detected.
  bool removeDoubleLcm();

  // Calculates a lower bound on the content of the slice (see
  // getLowerBound()) and takes the colon of getIdeal() and
  // getSubtract() by that while multiplying it on to
  // getMultiply(). Returns true if this changed the slice in any way,
  // i.e. if the lower bound was different from the identity.
  //
  // Note that it can pay off to apply the lower bound repeatedly.
  bool applyLowerBound();

 private:
  // Calculates the gcd of those generators of getIdeal() that are
  // divisible by var. This gcd is then divided by var to yield a
  // lower bound on the content of the slice. Returns false if a base
  // case is detected.
  bool getLowerBound(Term& bound, size_t var) const;

  // Calculates the lcm of the lower bounds from the getLowerBound
  // that takes a variable. This is a lower bound on the content of
  // the slice. Returns false if a base case is detected.
  bool getLowerBound(Term& bound) const;

  // Returns true if getGeneratorCount() returns 2, in which case the
  // content of the slice is output to consumer. It is a prerequisite
  // that the slice is fully simplified and that getIdeal() has more
  // than a single generator.
  bool twoVarBaseCase(DecomConsumer* consumer);

  size_t _varCount;
  Term _multiply;
  mutable Term _lcm;


  Ideal _ideal;
  Ideal _subtract;
};

namespace std {
  // This allows STL to swap slices more efficiently.
  template<> inline void swap<Slice>(Slice& a, Slice& b) {
    a.swap(b);
  }
}


#endif
