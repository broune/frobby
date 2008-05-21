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
#ifndef HILBERT_SLICE_GUARD
#define HILBERT_SLICE_GUARD

#include "Slice.h"
#include "Term.h"
#include "Ideal.h"

class CoefTermConsumer;

// TODO: fix code duplication from Slice

class HilbertSlice : public Slice {
 public:
  HilbertSlice();
  HilbertSlice(const Ideal& ideal,
			   const Ideal& subtract,
			   const Term& multiply,
			   CoefTermConsumer* consumer);

  // *** Mutators

  // Efficiently swaps the values of *this and slice while avoiding
  // copies.
  void swap(HilbertSlice& slice);

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

  // Returns true if a base case is reached, and in that case outputs
  // the content to consumer. The slice must be fully simplified.
  //
  // A base case is reached if not all variables divide the lcm of
  // getIdeal(), or if getGeneratorCount() is 2, or if getIdeal() is
  // square free.
  bool baseCase();

  // Simplies the slice. It is a precondition that the slice is
  // already normalized.
  void simplify();

  // Like simplify(), except that only one simplification step is
  // performed. If the return value is true, then the Slice may not be
  // fully simplified yet. Iterating simplifyStep() has the same
  // result as calling simplify(), though the performance
  // characteristics can be different.
  bool simplifyStep();

 private:
  // Calculates a lower bound on the content of the slice (see
  // getLowerBound()) and calls innerSlice with that lower bound. This
  // is repeated until a fixed point is reached. Returns false if no
  // minimal generator of getIdeal() or getSubtract() has had their
  // support changed or if an empty base case is detected.
  bool applyLowerBound();

  // Returns a lower bound on the content of the slice. Returns false
  // if a base case is detected.
  bool getLowerBound(Term& bound, size_t var) const;

  // Calculates the lcm of the lower bounds from the getLowerBound
  // that takes a variable. This is a lower bound on the content of
  // the slice. Returns false if an empty base case is detected.
  bool getLowerBound(Term& bound) const;

  CoefTermConsumer* _consumer;
};

namespace std {
  // This allows STL to swap slices more efficiently.
  template<> inline void swap<HilbertSlice>(HilbertSlice& a, HilbertSlice& b) {
    a.swap(b);
  }
}

#endif
