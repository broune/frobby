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

void runSliceAlgorithm(const Ideal& ideal, SliceStrategy& strategy) {
  // This is a worklist algorithm with two different kinds of work
  // items - events and slices. This is represented by the two vectors
  // events and slices. If the next item in slices is 0, then the next
  // item to be processed is taken from events, and the 0 is removed
  // from slices.
  //
  // Note that the order of processing is important, since the
  // contract of the split() method completely specifies
  // it. E.g. events can be used to free resources, and it would be
  // bad to do that before those resources were done being used.
  //
  // Also note that it is required to call dispose() on each event,
  // even if an exception occurs in the middle of the computation so
  // that raiseEvent() does not get called. In case of an exception,
  // deallocation of slices and dispose() on events should still be
  // called in the correct sequence.

  vector<SliceEvent*> events;
  vector<Slice*> slices;

  try {
	exceptionSafePushBack(slices, strategy.beginComputing(ideal));

	while (!slices.empty()) {
	  auto_ptr<Slice> slice(slices.back());
	  slices.pop_back();

	  if (slice.get() == 0) {
		SliceEvent* event = events.back();
		event->raiseEvent();
		events.pop_back();
		event->dispose();
		continue;
	  }

	  if (strategy.processIfBaseCase(*slice)) {
		strategy.freeSlice(slice);
		continue;
	  }

	  SliceEvent* leftEvent = 0;
	  SliceEvent* rightEvent = 0;
	  auto_ptr<Slice> leftSlice;
	  auto_ptr<Slice> rightSlice;
	  strategy.split(slice,
					 leftEvent, leftSlice,
					 rightEvent, rightSlice);

	  try {
		if (leftEvent != 0) {
		  slices.push_back(0);
		  try {
			events.push_back(leftEvent);
		  } catch (...) {
			slices.pop_back();
			throw;
		  }
		  leftEvent = 0;
		}		  

		if (leftSlice.get() != 0)
		  exceptionSafePushBack(slices, leftSlice);

		if (rightEvent != 0) {
		  slices.push_back(0);
		  try {
			events.push_back(rightEvent);
		  } catch (...) {
			slices.pop_back();
			throw;
		  }
		  rightEvent = 0;
		}

		if (rightSlice.get() != 0)
		  exceptionSafePushBack(slices, rightSlice);

		ASSERT(leftEvent == 0);
		ASSERT(rightEvent == 0);
	  } catch (...) {
		if (leftEvent != 0)
		  leftEvent->dispose();
		if (rightEvent != 0)
		  rightEvent->dispose();
		throw;
	  }
	}
  } catch (...) {
	// Deallocate and dispose() in correct order.
	while (!slices.empty()) {
	  if (slices.back() == 0) {
		ASSERT(!events.empty());
		events.back()->dispose();
		events.pop_back();
	  } else
		delete slices.back();
	  slices.pop_back();
	}

	ASSERT(slices.empty());
	ASSERT(events.empty());
	throw;
  }

  strategy.doneComputing();
}
