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
#ifndef SLICE_STRATEGY_COMMON
#define SLICE_STRATEGY_COMMON

#include "SliceStrategy.h"
#include "SplitStrategy.h"

#include <vector>
#include <string>
#include "Term.h"

class Slice;
class SplitStrategy;

// This class adds code to the SliceStrategy base class that is useful
// for derived classes.
class SliceStrategyCommon : public SliceStrategy {
 public:
  SliceStrategyCommon(const SplitStrategy* splitStrategy);
  virtual ~SliceStrategyCommon();

  virtual void freeSlice(auto_ptr<Slice> slice);

  virtual void setUseIndependence(bool use);

 protected:
  // Directly allocate a slice of the correct type using new.
  virtual auto_ptr<Slice> allocateSlice() = 0;

  // Check that this slice is valid for use with this strategy. No
  // check need be performed unless DEBUG is defined, making it
  // acceptable to check things using ASSERT. This method should not
  // be called if DEBUG is not defined.
  virtual bool debugIsValidSlice(Slice* slice) = 0;

  // Returns a slice from the cache that freeSlice adds to, or
  // allocate a new one using allocateSlice. This method should be
  // used in place of allocating new slices directly.
  auto_ptr<Slice> newSlice();

  // Takes over ownership of slice and populates leftSlice and
  // rightSlice with simplified sub-slices. Uses the pivot gotten
  // through getPivot.
  virtual void pivotSplit(auto_ptr<Slice> slice,
						  auto_ptr<Slice>& leftSlice,
						  auto_ptr<Slice>& rightSlice);

  // Used by pivotSplit to obtain a pivot.
  virtual void getPivot(Term& pivot, Slice& slice) = 0;

  bool getUseIndependence() const;

  const SplitStrategy* _split;

 private:
  bool _useIndependence;

  // This is the cache maintained through newSlice and freeSlice. It
  // would make more sense with a stack, but that class has
  // (surprisingly) proven to have too high overhead.
  vector<Slice*> _sliceCache;

  Term _pivotTmp;
};

#endif
