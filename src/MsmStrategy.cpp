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
#include "MsmStrategy.h"

#include "MsmSlice.h"
#include "Term.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include <vector>
#include "Projection.h"
#include "SliceAlgorithm.h"
#include "TermGrader.h"
#include "SliceEvent.h"

Slice* MsmStrategy::setupInitialSlice(const Ideal& ideal) {
  size_t varCount = ideal.getVarCount();

  Term sliceMultiply(varCount);
  for (size_t var = 0; var < varCount; ++var)
	sliceMultiply[var] = 1;

  Slice* slice = new MsmSlice(ideal, Ideal(varCount), sliceMultiply,
							  _consumer);
  slice->simplify();

  return slice;
}

MsmSlice* MsmStrategy::newMsmSlice() {
  Slice* slice = newSlice();
  ASSERT(dynamic_cast<MsmSlice*>(slice) != 0);
  return static_cast<MsmSlice*>(slice);
}

Slice* MsmStrategy::allocateSlice() {
  return new MsmSlice();
}

bool MsmStrategy::debugIsValidSlice(Slice* slice) {
  ASSERT(slice != 0);
  ASSERT(dynamic_cast<MsmSlice*>(slice) != 0);
  return true;
}

void MsmStrategy::labelSplit(Slice* slice,
							 Slice*& leftSlice, Slice*& rightSlice) {
  ASSERT(!slice->normalize());
  ASSERT(_split != 0);
  size_t var = _split->getLabelSplitVariable(*slice);

  Term term(slice->getVarCount());

  const Term& lcm = slice->getLcm();

  Ideal::const_iterator stop = slice->getIdeal().end();
  Ideal::const_iterator label = stop;
  bool hasTwoLabels = false;
  for (Ideal::const_iterator it = slice->getIdeal().begin();
	   it != stop; ++it) {
    if ((*it)[var] == 1) {
	  term = *it;
	  term[var] -= 1;

	  bool couldBeLabel = !slice->getSubtract().contains(term);
	  if (couldBeLabel) {
		for (size_t v = 0; v < slice->getVarCount(); ++v) {
		  if (term[v] == lcm[v]) {
			couldBeLabel = false;
			break;
		  }
		}
	  }

	  if (couldBeLabel) {
		if (label == stop)
		  label = it;
		else {
		  hasTwoLabels = true;
		  break;
		}
	  }
	}
  }

  Slice* hasLabelSlice = 0;

  if (label != stop) {
	term = *label;
	term[var] -= 1;

	hasLabelSlice = newSlice();
	*hasLabelSlice = *slice;
	hasLabelSlice->innerSlice(term);

	if (hasTwoLabels)
	  slice->outerSlice(term);
  }

  if (!hasTwoLabels) {
	term.setToIdentity();
	term[var] = 1;
	slice->innerSlice(term);
  }

  leftSlice = hasLabelSlice;
  if (leftSlice != 0)
	leftSlice->simplify();

  rightSlice = slice;
  rightSlice->simplify();
}

MsmStrategy::MsmStrategy(TermConsumer* consumer, const SplitStrategy* split):
  SliceStrategyCommon(split),
  _consumer(consumer) {
  ASSERT(consumer != 0);
}

MsmStrategy::~MsmStrategy() {
}

class MsmIndependenceSplit : public TermConsumer, public SliceEvent {
public:
  SliceEvent* getLeftEvent() {
	return this;
  }

  TermConsumer* getLeftConsumer() {
	return this;
  }

  TermConsumer* getRightConsumer() {
	return &_rightConsumer;
  }

  const Projection& getLeftProjection() {
	return _leftProjection;
  }

  const Projection& getRightProjection() {
	return _rightProjection;
  }

  void reset(TermConsumer* consumer,
			 IndependenceSplitter& splitter) {
	_consumer = consumer;
	_tmpTerm.reset(splitter.getVarCount());

	splitter.getBigProjection(_leftProjection);
	splitter.getRestProjection(_rightProjection);

	_rightConsumer._decom.clearAndSetVarCount
	  (_rightProjection.getRangeVarCount());	
  }

private:
  virtual void raiseEvent() {
	delete this;
  }

  virtual void consume(const Term& term) {
	_leftProjection.inverseProject(_tmpTerm, term);
	Ideal::const_iterator stop = _rightConsumer._decom.end();
	for (Ideal::const_iterator it = _rightConsumer._decom.begin();
		 it != stop; ++it) {
	  _rightProjection.inverseProject(_tmpTerm, *it);
	  _consumer->consume(_tmpTerm);
	}
  }

  struct RightConsumer : public TermConsumer {
	virtual void consume(const Term& term) {
	  _decom.insert(term);
	}

	Ideal _decom;
  } _rightConsumer;

  TermConsumer* _consumer;

  Projection _leftProjection;
  Projection _rightProjection;

  Term _tmpTerm;
};

bool MsmStrategy::independenceSplit
(MsmSlice* slice,
 SliceEvent*& leftEvent, Slice*& leftSlice,
 SliceEvent*& rightEvent, Slice*& rightSlice) {

  if (!_indep.analyze(*slice))
    return false;

  MsmIndependenceSplit* events = new MsmIndependenceSplit();
  events->reset(slice->getConsumer(), _indep);

  leftEvent = events;
  MsmSlice* msmLeftSlice = new MsmSlice();
  msmLeftSlice->setToProjOf(*slice, events->getLeftProjection(), events);
  leftSlice = msmLeftSlice;

  MsmSlice* msmRightSlice = new MsmSlice();
  msmRightSlice->setToProjOf(*slice, events->getRightProjection(),
							 events->getRightConsumer());
  rightSlice = msmRightSlice;

  freeSlice(slice);

  return true;
}

void MsmStrategy::split(Slice* sliceParam,
						SliceEvent*& leftEvent, Slice*& leftSlice,
						SliceEvent*& rightEvent, Slice*& rightSlice) {
  ASSERT(sliceParam != 0);
  ASSERT(dynamic_cast<MsmSlice*>(sliceParam) != 0);
  MsmSlice* slice = static_cast<MsmSlice*>(sliceParam);

  ASSERT(leftEvent == 0);
  ASSERT(leftSlice == 0);
  ASSERT(rightEvent == 0);
  ASSERT(rightSlice == 0);

  if (getUseIndependence() && independenceSplit
	  (slice, leftEvent, leftSlice, rightEvent, rightSlice))
	return;

  if (_split->isLabelSplit())
	labelSplit(slice, leftSlice, rightSlice);
  else {
	ASSERT(_split->isPivotSplit());
	pivotSplit(slice, leftSlice, rightSlice);
  }
}

void MsmStrategy::getPivot(Term& pivot, Slice& slice) {
  ASSERT(_split != 0);
  ASSERT(_split->isPivotSplit());

  _split->getPivot(pivot, slice);
}
