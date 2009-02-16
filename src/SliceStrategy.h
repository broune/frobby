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
#ifndef SLICE_STRATEGY_GUARD
#define SLICE_STRATEGY_GUARD

class Slice;
class Term;
class SliceEvent;
class Ideal;

// This class describes the interface of a strategy object for the
// Slice Algorithm. It determines what goes on when the algorithm
// runs, allowing to specialize the algorithm to do several different
// computations.
class SliceStrategy {
 public:
  virtual ~SliceStrategy();

  // This method should only be called before calling beginComputing().
  virtual void setUseIndependence(bool use) = 0;

  // This returns a slice based on ideal, which can be used to start
  // the computation off. This method should only be called once per
  // strategy, and it should be called before split().
  virtual auto_ptr<Slice> beginComputing(const Ideal& ideal) = 0;

  // This should be called once after computation is done, and then no
  // more methods other than the destructor should be called.
  virtual void doneComputing() = 0;

  // Performs a split of slice and puts the output into the remaining
  // four parameters. The strategy takes over ownership of slice,
  // while passing on ownership of leftSlice and rightSlice. Ownership
  // of leftEvent and rightEvent is not passed on, though it is
  // required that if non-null, then their dispose() method must be
  // called eventually.
  //
  // The parameter slice must have been obtained through a method of
  // this strategy - it must not have been allocated using new or by a
  // different strategy.
  //
  // The algorithm must process rightSlice first, then raise
  // rightEvent, then process leftSlice and finally raise leftEvent.
  // Processing includes processing any child slices generated from
  // further splits. Any of leftEvent, leftSlice, rightEvent and
  // rightSlice can be 0 after split returns, in which case that
  // output is to be ignored.
  //
  // leftEvent, leftSlice, rightEvent and rightSlice are only used to
  // produce output from split. To make this point clear, they are
  // required to be 0 when split gets called. Slice is not allowed to
  // be 0.
  virtual void split(auto_ptr<Slice> slice,
					 SliceEvent*& leftEvent, auto_ptr<Slice>& leftSlice,
					 SliceEvent*& rightEvent, auto_ptr<Slice>& rightSlice) = 0;

  // It is allowed to delete returned slices directly, but it is
  // better to use freeSlice. freeSlice can only be called on slices
  // obtained from a method of the same strategy. This allows caching
  // of slices to avoid frequent allocations and deallocation.
  virtual void freeSlice(auto_ptr<Slice> slice) = 0;
};

#endif
