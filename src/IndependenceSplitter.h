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
  // slice must be simplified and normalized.
  IndependenceSplitter(const Partition& partition, MsmSlice* slice);

  static bool computePartition(Partition& partition, const MsmSlice* slice);

  MsmSlice& getLeftSlice() {return _leftSlice;}
  Projection& getLeftProjection() {return _leftProjection;}

  MsmSlice& getRightSlice() {return _rightSlice;}
  Projection& getRightProjection() {return _rightProjection;}

 private:
  MsmSlice _leftSlice;
  Projection _leftProjection;

  MsmSlice _rightSlice;
  Projection _rightProjection;
};

#endif
