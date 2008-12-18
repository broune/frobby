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
#ifndef INDEPENDENCE_SPLITTER_GUARD
#define INDEPENDENCE_SPLITTER_GUARD

#include "Partition.h"
#include "Projection.h"
#include "Ideal.h"
#include <vector>
#include "MsmSlice.h"

class Ideal;
class Term;

class IndependenceSplitter {
 public:
  // Returns true if there are independent subsets of variables. The
  // other methods should only be called when the most recent call to
  // analyze returned true.
  bool analyze(const Slice& slice);

  size_t getVarCount() const;

  size_t getOneVarCount() const {return _oneVarCount;}
  size_t getTwoVarCount() const {return _twoVarCount;}
  size_t getMoreThanTwoCount() const {return _moreThanTwoVarCount;}

  // Get the projection to the biggest independent subset of variables.
  void getBigProjection(Projection& projection) const;

  // Get the projection to the rest of the variables.
  void getRestProjection(Projection& projection) const;

 private:
  Partition _partition;

  size_t _oneVarCount;
  size_t _twoVarCount;
  size_t _moreThanTwoVarCount;

  size_t _bigSet;
};

#endif
