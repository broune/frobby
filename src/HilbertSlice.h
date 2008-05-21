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

  // Returns true if a base case is reached, and in that case outputs
  // the content to consumer. The slice must be fully simplified.
  //
  // A base case is reached if not all variables divide the lcm of
  // getIdeal(), or if getGeneratorCount() is 2, or if getIdeal() is
  // square free.
  bool baseCase();

  virtual void simplify();
  virtual bool simplifyStep();

  // Efficiently swaps the values of *this and slice while avoiding
  // copies.
  void swap(HilbertSlice& slice);

 private:
  // Returns a lower bound on the content of the slice. Returns false
  // if a base case is detected.
  virtual bool getLowerBound(Term& bound, size_t var) const;

  CoefTermConsumer* _consumer;
};

namespace std {
  // This allows STL to swap slices more efficiently.
  template<> inline void swap<HilbertSlice>(HilbertSlice& a, HilbertSlice& b) {
    a.swap(b);
  }
}

#endif
