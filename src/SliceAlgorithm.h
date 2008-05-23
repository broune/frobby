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
#ifndef SLICE_ALGORITHM_GUARD
#define SLICE_ALGORITHM_GUARD

class Ideal;
class MsmSlice;
class MsmStrategy;
class Term;

// Computes the Maximal Standard Monomials (MSMs) of the input ideal.
class SliceAlgorithm {
 public:
  SliceAlgorithm();

  // setStrategy takes over ownership of the strategy.
  void setStrategy(MsmStrategy* strategy);
  void setUseIndependence(bool useIndependence);

  // Runs the algorithm and clears ideal. Then deletes the strategy
  // and consumer.
  void runAndClear(Ideal& ideal);

 private:
  void content(MsmSlice* slice);

  bool _useIndependence;
  MsmStrategy* _strategy;
};

// For getting a single Maximal Standard Monomial. This may fail if
// slice.getIdeal() is not artinian, in which case the return value is
// false.
bool computeSingleMSM(const MsmSlice& slice, Term& msm);

#endif
