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
#include "HilbertStrategy.h"

#include "Term.h"
#include "HilbertSlice.h"
#include "Ideal.h"
#include "CoefTermConsumer.h"
#include "Projection.h"
#include "IndependenceSplitter.h"
#include "SliceEvent.h"
#include "HilbertIndependenceConsumer.h"

HilbertStrategy::HilbertStrategy(CoefTermConsumer* consumer,
								 const SplitStrategy* split):
  SliceStrategyCommon(split),
  _consumer(consumer) {
}

HilbertStrategy::~HilbertStrategy() {
  while (!_consumerCache.empty()) {
	delete _consumerCache.back();
	_consumerCache.pop_back();
  }
}

Slice* HilbertStrategy::setupInitialSlice(const Ideal& ideal) {
  ASSERT(_consumer != 0);

  size_t varCount = ideal.getVarCount();
  Ideal sliceIdeal(varCount);

  if (!ideal.contains(Term(varCount))) {
	_consumer->consume(1, Term(varCount));

	if (ideal.getGeneratorCount() > 0) {
	  Term allOnes(varCount);
	  for (size_t var = 0; var < varCount; ++var)
		allOnes[var] = 1;

	  sliceIdeal = ideal;
	  sliceIdeal.product(allOnes);
	}
  }

  HilbertSlice* slice = new HilbertSlice(sliceIdeal, Ideal(varCount),
										 Term(varCount), _consumer);
  slice->simplify();
  return slice;
}

void HilbertStrategy::
split(Slice* sliceParam,
	  SliceEvent*& leftEvent, Slice*& leftSlice,
	  SliceEvent*& rightEvent, Slice*& rightSlice) {
  ASSERT(sliceParam != 0);
  HilbertSlice* slice = static_cast<HilbertSlice*>(sliceParam);

  ASSERT(leftEvent == 0);
  ASSERT(leftSlice == 0);
  ASSERT(rightEvent == 0);
  ASSERT(rightSlice == 0);

  if (getUseIndependence() &&
	  independenceSplit(slice, leftEvent, leftSlice, rightSlice))
	return;

  ASSERT(_split->isPivotSplit());
  pivotSplit(slice, leftSlice, rightSlice);
}

HilbertSlice* HilbertStrategy::newHilbertSlice() {
  Slice* slice = newSlice();
  ASSERT(debugIsValidSlice(slice));
  return static_cast<HilbertSlice*>(slice);
}

Slice* HilbertStrategy::allocateSlice() {
  return new HilbertSlice();
}

bool HilbertStrategy::debugIsValidSlice(Slice* slice) {
  ASSERT(slice != 0);
  ASSERT(dynamic_cast<HilbertSlice*>(slice) != 0);
  return true;
}

void HilbertStrategy::getPivot(Term& term, Slice& slice) {
  ASSERT(term.getVarCount() == slice.getVarCount());

  _split->getPivot(term, slice);
}

void HilbertStrategy::freeConsumer(HilbertIndependenceConsumer* consumer) {
  ASSERT(consumer != 0);
  ASSERT(std::find(_consumerCache.begin(),
				   _consumerCache.end(), consumer) ==
		 _consumerCache.end());

  _consumerCache.push_back(consumer);
}

bool HilbertStrategy::independenceSplit(HilbertSlice* slice,
										SliceEvent*& leftEvent,
										Slice*& leftSlice,
										Slice*& rightSlice) {
  ASSERT(slice != 0);

  if (!_indepSplitter.analyze(*slice))
	return false;

  HilbertIndependenceConsumer* consumer = newConsumer();
  consumer->reset(slice->getConsumer(), _indepSplitter, slice->getVarCount());
  leftEvent = consumer;

  HilbertSlice* hilbertLeftSlice = newHilbertSlice();
  hilbertLeftSlice->setToProjOf(*slice, consumer->getLeftProjection(),
								consumer->getLeftConsumer());
  leftSlice = hilbertLeftSlice;

  HilbertSlice* hilbertRightSlice = newHilbertSlice();
  hilbertRightSlice->setToProjOf(*slice, consumer->getRightProjection(),
								 consumer->getRightConsumer());
  rightSlice = hilbertRightSlice;

  freeSlice(slice);

  return true;
}

HilbertIndependenceConsumer* HilbertStrategy::newConsumer() {
  if (_consumerCache.empty())
	return new HilbertIndependenceConsumer(this);

  HilbertIndependenceConsumer* consumer = _consumerCache.back();
  _consumerCache.pop_back();
  return consumer;
}
