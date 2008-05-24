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
#ifndef MSM_SLICE_GUARD
#define MSM_SLICE_GUARD

#include "Slice.h"
#include "Term.h"
#include "Ideal.h"

class TermConsumer;
class Projection;

class MsmSlice : public Slice {
 public:
  MsmSlice();
  MsmSlice(const Ideal& ideal,
		   const Ideal& subtract,
		   const Term& multiply,
		   TermConsumer* consumer);

  TermConsumer* getConsumer() {return _consumer;}

  // *** Mutators

  // A base case is reached if not all variables divide the lcm of
  // getIdeal(), or if getGeneratorCount() is 2, or if getIdeal() is
  // square free.
  virtual bool baseCase();

  virtual Slice& operator=(const Slice& slice);

  virtual void simplify();
  virtual bool simplifyStep();

  void setToProjOf(const MsmSlice& slice,
				   const Projection& projection,
				   TermConsumer* consumer);

  // Efficiently swaps the values of *this and slice while avoiding
  // copies.
  void swap(MsmSlice& slice);

 private:
  // Removes those generators g of getIdeal() such that g[i] equals
  // getLcm()[i] for two distinct i. This is done iteratively until no
  // more generators can be removed in this way. Returns true if any
  // generators were removed.
  bool removeDoubleLcm();

  // Calculates the gcd of those generators of getIdeal() that are
  // divisible by var. This gcd is then divided by var to yield a
  // lower bound on the content of the slice. Returns false if a base
  // case is detected. The real functionality is slight more
  // sophisticated.
  virtual bool getLowerBound(Term& bound, size_t var) const;

  // Outputs the content of the slice to consumer. It is a
  // precondition that the slice is fully simplified and that
  // getVarCount() returns 2.
  void twoVarBaseCase();

  // Outputs the content of the slice to consumer. It is a
  // precondition that the slice is fully simplified and that
  // getVarCount() plus one equals
  // getIdeal().getGeneratorCount(). This will, due to simplification,
  // be true if there is exactly one generator that is nowhere equal
  // to the lcm of getIdeal().
  void oneMoreGeneratorBaseCase();

  // Outputs the content of the slice to consumer or returns false. It
  // is a precondition that the slice is fully simplified. Returns
  // true if there are exactly two generators that are nowhere equal
  // to the lcm of getIdeal().
  bool twoNonMaxBaseCase();

  TermConsumer* _consumer;
};

namespace std {
  // This allows STL to swap slices more efficiently.
  template<> inline void swap<MsmSlice>(MsmSlice& a, MsmSlice& b) {
    a.swap(b);
  }
}

#endif
