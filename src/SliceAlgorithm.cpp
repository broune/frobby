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
#include "stdinc.h"
#include "SliceAlgorithm.h"

#include "Slice.h"
#include "MsmStrategy.h"
#include "HilbertStrategy.h"
#include "SliceEvent.h"
#include "DebugStrategy.h"
#include "ElementDeleter.h"

void runSliceAlgorithm(const Ideal& ideal, SliceStrategy* strategy) {
  ASSERT(strategy != 0);

  // TODO: describe the interplay between events and slices below.
  vector<SliceEvent*> events;
  vector<Slice*> slices;
  ElementDeleter<vector<Slice*> > slicesElementDeleter(slices);

  exceptionSafePushBack(slices, strategy->beginComputing(ideal));

  while (!slices.empty()) {
	auto_ptr<Slice> slice(slices.back());
	slices.pop_back();

	if (slice.get() == 0) {
	  events.back()->raiseEvent();
	  events.pop_back();
	  continue;
	}

	if (slice->baseCase()) {
	  strategy->freeSlice(slice);
	  continue;
	}

	SliceEvent* leftEvent = 0;
	SliceEvent* rightEvent = 0;
	auto_ptr<Slice> leftSlice;
	auto_ptr<Slice> rightSlice;
	strategy->split(slice,
					leftEvent, leftSlice,
					rightEvent, rightSlice);

	if (leftEvent != 0) {
	  slices.push_back(0);
	  events.push_back(leftEvent);
	}

	if (leftSlice.get() != 0)
	  exceptionSafePushBack(slices, leftSlice);

	if (rightEvent != 0) {
	  slices.push_back(0);
	  events.push_back(rightEvent);
	}

	if (rightSlice.get() != 0)
	  exceptionSafePushBack(slices, rightSlice);
  }

  strategy->doneComputing();
}
