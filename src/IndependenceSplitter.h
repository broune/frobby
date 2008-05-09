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
#include "Slice.h"

class Ideal;
class Term;

class IndependenceSplitter {
 public:
  // Slice must be simplified and normalized.
  IndependenceSplitter(const Partition& partition, Slice& slice);
  ~IndependenceSplitter();

  static void computePartition(Partition& partition, const Slice& slice);
  static bool shouldPerformSplit(const Partition& partition,
				 const Slice& slice);

  size_t getChildCount() const;
  Ideal* getMixedProjectionSubtract();

  Slice& getSlice(size_t part);
  Projection& getProjection(size_t part);

 private:
  struct Child {
    Slice slice;
    Projection projection;

    void swap(Child& child);
    bool operator<(const Child& child) const;
  };

  void initializeChildren(const Partition& partition);
  void populateChildIdealsAndSingletonDecom(const vector<Child*>& childAt);
  void populateChildSubtracts(const vector<Child*>& childAt);

  Slice& _slice;
  vector<Child> _children;
  Ideal* _mixedProjectionSubtract;
};

#endif
