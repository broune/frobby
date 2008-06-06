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
#include "IndependenceSplitter.h"
#include "SliceEvent.h"

Slice* MsmStrategy::setupInitialSlice(const Ideal& ideal) {
  size_t varCount = ideal.getVarCount();

  Term sliceMultiply(varCount);
  for (size_t var = 0; var < varCount; ++var)
	sliceMultiply[var] = 1;

  Slice* slice = new MsmSlice(ideal,
							  Ideal(varCount),
							  sliceMultiply,
							  _consumer);
  simplify(*slice);

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
  size_t var = getLabelSplitVariable(*slice);

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
  rightSlice = slice;
}

MsmStrategy::MsmStrategy(TermConsumer* consumer): 
  _consumer(consumer) {
}

MsmStrategy::~MsmStrategy() {
}

void MsmStrategy::setPivotStrategy(PivotStrategy pivotStrategy) {
  _splitStrategy = PivotSplit;
  _pivotStrategy = pivotStrategy;
  _labelStrategy = UnknownLabelStrategy;
}

void MsmStrategy::setLabelStrategy(LabelStrategy labelStrategy) {
  _splitStrategy = LabelSplit;
  _pivotStrategy = UnknownPivotStrategy;
  _labelStrategy = labelStrategy;
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

  static IndependenceSplitter indep;
  if (!indep.analyze(*slice))
    return false;

  MsmIndependenceSplit* events = new MsmIndependenceSplit();
  events->reset(slice->getConsumer(), indep);

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

  switch (_splitStrategy) {
  case MsmStrategy::LabelSplit:
	labelSplit(slice, leftSlice, rightSlice);
	break;

  case MsmStrategy::PivotSplit:
	pivotSplit(slice, leftSlice, rightSlice);
	break;

  default:
	ASSERT(false);
  }

  if (leftSlice != 0)
	simplify(*leftSlice);

  if (rightSlice != 0)
	simplify(*rightSlice);
}

void MsmStrategy::simplify(Slice& slice) {
  slice.simplify();
}

void MsmStrategy::getPivot(Term& pivot, Slice& slice) {
  ASSERT(_splitStrategy == PivotSplit);
  SliceStrategyCommon::getPivot(pivot, slice, _pivotStrategy);
}

size_t MsmStrategy::getLabelSplitVariable(const Slice& slice) {
  ASSERT(_splitStrategy == LabelSplit);

  Term co(slice.getVarCount());
  slice.getIdeal().getSupportCounts(co);

  if (_labelStrategy == MaxLabel) {
	// Return the variable that divides the most minimal generators.
	// This cannot be an invalid split because if every count was 1,
	// then this would be a base case.
	return co.getFirstMaxExponent();
  }

  // For each variable, count number of terms with exponent of 1
  Term co1(slice.getVarCount());
  Ideal::const_iterator end = slice.getIdeal().end();
  for (Ideal::const_iterator it = slice.getIdeal().begin();
	   it != end; ++it) {
	// This way we avoid bad splits.
	if (getSizeOfSupport(*it, slice.getVarCount()) == 1)
	  continue;
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  if ((*it)[var] == 1)
		co1[var] += 1;
  }

  // The slice is simplified and not a base case slice.
  ASSERT(!co1.isIdentity());

  if (_labelStrategy == VarLabel) {
	// Return the least variable that is valid.
	for (size_t var = 0; ; ++var) {
	  ASSERT(var < slice.getVarCount());
	  if (co1[var] > 0)
		return var;
	}
  }

  if (_labelStrategy != MinLabel) {
	fputs("INTERNAL ERROR: Undefined label split type.\n", stderr);
	ASSERT(false);
	exit(1);
  }

  // Zero those variables of co that have more than the least number
  // of exponent 1 minimal generators.
  size_t mostGeneric = 0;
  for (size_t var = 1; var < slice.getVarCount(); ++var)
	if (mostGeneric == 0 || (mostGeneric > co1[var] && co1[var] > 0))
	  mostGeneric = co1[var];
  for (size_t var = 0; var < slice.getVarCount(); ++var)
	if (co1[var] != mostGeneric)
	  co[var] = 0;

  // Among those with least exponent 1 minimal generators, return
  // the variable that divides the most minimal generators.
  return co.getFirstMaxExponent();
}

bool MsmStrategy::setSplitStrategy(const string& name) {
  if (name == "maxlabel")
	setLabelStrategy(MaxLabel);
  else if (name == "minlabel")
	setLabelStrategy(MinLabel);
  else if (name == "varlabel")
	setLabelStrategy(VarLabel);
  else {
	PivotStrategy pivotStrategy = SliceStrategyCommon::getPivotStrategy(name);
	if (pivotStrategy == UnknownPivotStrategy)
	  return false;

	setPivotStrategy(pivotStrategy);
  }
  return true;
}

MsmStrategy* MsmStrategy::newDecomStrategy(const string& name,
										   TermConsumer* consumer) {
  MsmStrategy* strategy = new MsmStrategy(consumer);
  if (!strategy->setSplitStrategy(name)) {
	fprintf(stderr, "ERROR: Unknown split strategy \"%s\".\n", name.c_str());
	exit(1);
  }
  return strategy;
}
