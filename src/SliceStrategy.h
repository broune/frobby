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

#include <string>

class SliceStrategy {
 public:
  virtual ~SliceStrategy() {}

  virtual void split(Slice* slice,
					 SliceEvent*& leftEvent, Slice*& leftSlice,
					 SliceEvent*& rightEvent, Slice*& rightSlice) = 0;

  virtual void freeSlice(Slice* slice) = 0;

 protected:
  enum PivotStrategy {
	Unknown, // Cannot be used to obtain pivots.
    Minimum,
    Median,
    Maximum,
	MinGen,
	Indep,
	GCD
  };

  static void getPivot(Term& pivot, Slice& slice, PivotStrategy ps);
  static PivotStrategy getPivotStrategy(const string& name);
};

#endif
