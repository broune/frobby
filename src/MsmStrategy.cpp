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

auto_ptr<Slice> MsmStrategy::beginComputing(const Ideal& ideal) {
  _consumer->beginConsuming();
  size_t varCount = ideal.getVarCount();

  Term sliceMultiply(varCount);
  for (size_t var = 0; var < varCount; ++var)
	sliceMultiply[var] = 1;

  auto_ptr<Slice> slice
	(new MsmSlice(ideal, Ideal(varCount), sliceMultiply, _consumer));
  simplify(*slice);
  return slice;
}

void MsmStrategy::doneComputing() {
  _consumer->doneConsuming();
}

auto_ptr<MsmSlice> MsmStrategy::newMsmSlice() {
  auto_ptr<Slice> slice(newSlice());
  ASSERT(dynamic_cast<MsmSlice*>(slice.get()) != 0);
  return auto_ptr<MsmSlice>(static_cast<MsmSlice*>(slice.release()));
}

auto_ptr<Slice> MsmStrategy::allocateSlice() {
  return auto_ptr<Slice>(new MsmSlice());
}

bool MsmStrategy::debugIsValidSlice(Slice* slice) {
  ASSERT(slice != 0);
  ASSERT(dynamic_cast<MsmSlice*>(slice) != 0);
  return true;
}

void MsmStrategy::labelSplit(auto_ptr<MsmSlice> slice,
							 auto_ptr<Slice>& leftSlice,
							 auto_ptr<Slice>& rightSlice) {
  ASSERT(!slice->adjustMultiply());
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

  auto_ptr<Slice> hasLabelSlice;

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
  if (leftSlice.get() != 0)
	simplify(*leftSlice);

  rightSlice = slice;
  simplify(*rightSlice);
}

MsmStrategy::MsmStrategy(TermConsumer* consumer,
						 const SplitStrategy* splitStrategy):
  SliceStrategyCommon(splitStrategy),
  _consumer(consumer) {
  ASSERT(consumer != 0);
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
  }

  virtual void dispose() {
	delete this;
  }

  virtual void beginConsuming() {
  }

  virtual void doneConsuming() {
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
	virtual void beginConsuming() {
	}

	virtual void doneConsuming() {
	}

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

void MsmStrategy::independenceSplit
(auto_ptr<MsmSlice> slice,
 SliceEvent*& leftEvent, auto_ptr<Slice>& leftSlice,
 SliceEvent*& rightEvent, auto_ptr<Slice>& rightSlice) {

  // Construct left event (assignment later).
  auto_ptr<MsmIndependenceSplit> events(new MsmIndependenceSplit());
  events->reset(slice->getConsumer(), _indep);

  // Construct left slice.
  auto_ptr<MsmSlice> msmLeftSlice(new MsmSlice());
  msmLeftSlice->setToProjOf(*slice, events->getLeftProjection(), events.get());
  leftSlice = msmLeftSlice;

  // Construct right slice.
  auto_ptr<MsmSlice> msmRightSlice(new MsmSlice());
  msmRightSlice->setToProjOf(*slice, events->getRightProjection(),
							 events->getRightConsumer());
  rightSlice = msmRightSlice;

  // Deal with slice.
  freeSlice(static_cast<auto_ptr<Slice> >(slice));

  // Done last to avoid memory leak on exception.
  leftEvent = events.release();
}

void MsmStrategy::split(auto_ptr<Slice> sliceParam,
						SliceEvent*& leftEvent, auto_ptr<Slice>& leftSlice,
						SliceEvent*& rightEvent, auto_ptr<Slice>& rightSlice) {
  ASSERT(sliceParam.get() != 0);
  ASSERT(dynamic_cast<MsmSlice*>(sliceParam.get()) != 0);
  auto_ptr<MsmSlice> slice(static_cast<MsmSlice*>(sliceParam.release()));

  ASSERT(leftEvent == 0);
  ASSERT(leftSlice.get() == 0);
  ASSERT(rightEvent == 0);
  ASSERT(rightSlice.get() == 0);

  if (getUseIndependence() && _indep.analyze(*slice)) {
	independenceSplit(slice, leftEvent, leftSlice, rightEvent, rightSlice);
	return;
  }

  if (_split->isLabelSplit())
	labelSplit(slice, leftSlice, rightSlice);
  else {
	ASSERT(_split->isPivotSplit());
	pivotSplit(auto_ptr<Slice>(slice), leftSlice, rightSlice);
  }
}

void MsmStrategy::getPivot(Term& pivot, Slice& slice) {
  ASSERT(_split != 0);
  ASSERT(_split->isPivotSplit());

  _split->getPivot(pivot, slice);
}

void MsmStrategy::getPivot(Term& pivot, Slice& slice, const TermGrader& grader) {
  ASSERT(_split != 0);
  ASSERT(_split->isPivotSplit());

  _split->getPivot(pivot, slice, grader);
}
