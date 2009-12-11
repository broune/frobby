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
#include "Task.h"
#include "TaskEngine.h"

namespace {
  class SliceEventTask : public Task {
  public:
	SliceEventTask(SliceEvent* event): _event(event) {
	  ASSERT(event != 0);
	}

	virtual ~SliceEventTask() {
	  ASSERT(_event == 0); // Destructed through dispose.
	}

	static void addTask(TaskEngine& tasks, SliceEvent* event) {
	  ASSERT(event != 0);

	  SliceEventTask* task;
	  try {
		task = new SliceEventTask(event);
	  } catch (...) {
		event->dispose();
		throw;
	  }

	  tasks.addTask(task);
	}

	virtual void run(TaskEngine& task) {
	  _event->raiseEvent();
	}

	virtual void dispose() {
	  ASSERT(_event != 0); // Only call dispose once.

	  _event->dispose();

	  IF_DEBUG(_event = 0); // Signals dispose was called for debugging.
	  delete this;
	}

  private:
	SliceEvent* _event;
  };
}

class SliceTask : public Task {
public:
  SliceTask(SliceStrategy& strategy, auto_ptr<Slice> slice):
	_strategy(strategy),
	_slice(slice) {
	ASSERT(_slice.get() != 0);
  }

  virtual ~SliceTask() {
  }

  static void addTask(TaskEngine& task,
					  SliceStrategy& strategy, auto_ptr<Slice> slice) {
	ASSERT(slice.get() != 0);

	task.addTask(new SliceTask(strategy, slice));
  }

  virtual void run(TaskEngine& tasks) {
	if (_strategy.processIfBaseCase(*_slice)) {
	  _strategy.freeSlice(_slice);
	  return;
	}

	SliceEvent* leftEvent = 0;
	SliceEvent* rightEvent = 0;
	auto_ptr<Slice> leftSlice;
	auto_ptr<Slice> rightSlice;
	_strategy.split(_slice,
					leftEvent, leftSlice,
					rightEvent, rightSlice);

	try {
	  if (leftEvent != 0)
		SliceEventTask::addTask(tasks, leftEvent);
	  if (leftSlice.get() != 0)
		SliceTask::addTask(tasks, _strategy, leftSlice);
	} catch (...) {
	  if (rightEvent != 0)
		rightEvent->dispose();
	  throw;
	}

	if (rightEvent != 0)
	  SliceEventTask::addTask(tasks, rightEvent);
	if (rightSlice.get() != 0)
	  SliceTask::addTask(tasks, _strategy, rightSlice);
  }

  virtual void dispose() {
	if (_slice.get() != 0)
	  _strategy.freeSlice(_slice);
	delete this;
  }

private:
  SliceStrategy& _strategy;
  auto_ptr<Slice> _slice;
};

void runSliceAlgorithmTask(const Ideal& ideal, SliceStrategy& strategy) {
  TaskEngine tasks;

  auto_ptr<Slice> initialSlice = strategy.beginComputing(ideal);
  tasks.addTask(initialSlice.release());
  //SliceTask::addTask(tasks, strategy, initialSlice);

  tasks.runTasks();

  strategy.doneComputing();
}

void runSliceAlgorithm(const Ideal& ideal, SliceStrategy& strategy) {
  return runSliceAlgorithmTask(ideal, strategy);

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
