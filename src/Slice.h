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
#ifndef SLICE_GUARD
#define SLICE_GUARD

#include "Ideal.h"
#include "Term.h"

class Projection;

class Slice {
 public:
  Slice();
  Slice(const Ideal& ideal, const Ideal& subtract, const Term& multiply);
  virtual ~Slice() {}

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

  void print(FILE* file) const;

  // *** Mutators

  // Returns true if a base case is reached, and in that case outputs
  // the content to consumer. The slice must be fully simplified.
  virtual bool baseCase() = 0;

  virtual Slice& operator=(const Slice& slice) = 0;

  // Removes all generators of getIdeal() and getSubtract() and sets
  // getMultiply() to the identity. Also changes getVarCount() to varCount.
  void resetAndSetVarCount(size_t varCount);

  // Clears getIdeal() and getSubtract(). Does not change getMultiply().
  void clearIdealAndSubtract();

  // Calls singleDegreeSort on getIdeal().
  void singleDegreeSortIdeal(size_t var);

  // Inserts term into getIdeal().
  void insertIntoIdeal(const Exponent* term);

  // Computes an inner slice with the specified pivot, i.e. a colon by
  // pivot is applied to getMultiply() and getSubtract(), while
  // getMultiply() is multiplied by pivot. The slice is then
  // normalized.
  //
  // Returns true if the colon operation was non-trivial in the sense
  // that it changed the support of any minimal generator of
  // getIdeal() or getSubtract().
  bool innerSlice(const Term& pivot);

  // Computes an outer slice with the specified pivot, i.e. strict
  // multiples of pivot are removed from getIdeal(), and pivot is
  // added to getSubtract() if necessary.
  void outerSlice(const Term& pivot);

  // Removes those generators of getIdeal() that are strict multiples
  // of some generator of getSubtract(). Returns true if any
  // generators were removed.
  bool normalize();

  // Simplies the slice such that normalize, pruneSubtract,
  // removeDoubleLcm and applyLowerBound all return false. It is a
  // precondition that the slice is already normalized.
  virtual void simplify() = 0;

  // Like simplify(), except that only one simplification step is
  // performed. If the return value is true, then the Slice may not be
  // fully simplified yet. Iterating simplifyStep() has the same
  // result as calling simplify(), though the performance
  // characteristics can be worse.
  virtual bool simplifyStep() = 0;

 protected:
  void setToProjOf(const Slice& slice, const Projection& projection);

  void swap(Slice& slice);

  // Removes those generators of subtract that do not strictly divide
  // the lcm of getIdeal(), or that lies within the ideal
  // getIdeal(). Returns true if any generators were removed.
  bool pruneSubtract();

  // Calculates a lower bound on the content of the slice (see
  // getLowerBound()) and calls innerSlice with that lower bound. This
  // is repeated until a fixed point is reached. Returns false if no
  // minimal generator of getIdeal() or getSubtract() has had their
  // support changed or if an empty base case is detected.
  bool applyLowerBound();

  // Calculates the gcd of those generators of getIdeal() that are
  // divisible by var. This gcd is then divided by var to yield a
  // lower bound on the content of the slice. Returns false if a base
  // case is detected. The real functionality is slight more
  // sophisticated.
  virtual bool getLowerBound(Term& bound, size_t var) const = 0;

  size_t _varCount;
  Term _multiply;

  mutable Term _lcm;
  mutable bool _lcmUpdated;

  Ideal _ideal;
  Ideal _subtract;

  size_t _lowerBoundHint;
};

#endif
