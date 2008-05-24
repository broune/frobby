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
#ifndef HILBERT_STRATEGY_GUARD
#define HILBERT_STRATEGY_GUARD

#include <vector>
#include "Term.h"

class HilbertSlice;
class Ideal;
class CoefTermConsumer;
class SliceEvent;
class HilbertIndependenceConsumer;
class Slice;

#include "SliceStrategy.h"

class HilbertStrategy : public SliceStrategy {
 public:
  HilbertStrategy(bool useIndependence = false);
  virtual ~HilbertStrategy();

  Slice* setupInitialSlice(const Ideal& ideal,
						   CoefTermConsumer* consumer);

  // Takes over ownership of the slice parameter.
  virtual void split(Slice* slice,
					 SliceEvent*& leftEvent, Slice*& leftSlice,
					 SliceEvent*& rightEvent, Slice*& rightSlice);

  virtual void freeSlice(Slice* slice);
  void freeConsumer(HilbertIndependenceConsumer* consumer);

 private:
  HilbertSlice* newSlice();
  HilbertIndependenceConsumer* newConsumer();

  void getPivot(Term& term, Slice& slice);

  bool independenceSplit(HilbertSlice* slice,
						 SliceEvent*& leftEvent,
						 Slice*& leftSlice,
						 Slice*& rightSlice);


  Term _term;

  // It would make more sense with a stack, but that class has
  // (surprisingly) proven to have too high overhead.
  vector<Slice*> _sliceCache;
  vector<HilbertIndependenceConsumer*> _consumerCache;

  bool _useIndependence;
};

#endif
