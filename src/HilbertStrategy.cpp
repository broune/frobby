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
#include "ElementDeleter.h"

HilbertStrategy::HilbertStrategy(CoefTermConsumer* consumer,
								 const SplitStrategy* splitStrategy):
  SliceStrategyCommon(splitStrategy),
  _consumerCache(),
  _consumerCacheDeleter(_consumerCache),
  _consumer(consumer) {
}

auto_ptr<Slice> HilbertStrategy::beginComputing(const Ideal& ideal) {
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

  auto_ptr<Slice> slice
	(new HilbertSlice(sliceIdeal, Ideal(varCount),
					  Term(varCount), _consumer));

  simplify(*slice);
  return slice;
}

void HilbertStrategy::doneComputing() {
  _consumerCacheDeleter.deleteElements();
}

void HilbertStrategy::
split(auto_ptr<Slice> sliceParam,
	  SliceEvent*& leftEvent, auto_ptr<Slice>& leftSlice,
	  SliceEvent*& rightEvent, auto_ptr<Slice>& rightSlice) {
  ASSERT(sliceParam.get() != 0);
  auto_ptr<HilbertSlice> slice
	(static_cast<HilbertSlice*>(sliceParam.release()));

  ASSERT(leftEvent == 0);
  ASSERT(leftSlice.get() == 0);
  ASSERT(rightEvent == 0);
  ASSERT(rightSlice.get() == 0);

  if (getUseIndependence() && _indepSplitter.analyze(*slice)) {
	independenceSplit(slice, leftEvent, leftSlice, rightSlice);
	return;
  }

  ASSERT(_split->isPivotSplit());
  pivotSplit(auto_ptr<Slice>(slice), leftSlice, rightSlice);
}

auto_ptr<HilbertSlice> HilbertStrategy::newHilbertSlice() {
  auto_ptr<Slice> slice(newSlice());
  ASSERT(debugIsValidSlice(slice.get()));
  return auto_ptr<HilbertSlice>(static_cast<HilbertSlice*>(slice.release()));
}

auto_ptr<Slice> HilbertStrategy::allocateSlice() {
  return auto_ptr<Slice>(new HilbertSlice());
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

void HilbertStrategy::freeConsumer(auto_ptr<HilbertIndependenceConsumer>
								   consumer) {
  ASSERT(consumer.get() != 0);
  ASSERT(std::find(_consumerCache.begin(),
				   _consumerCache.end(), consumer.get()) ==
		 _consumerCache.end());

  consumer->clear();
  noThrowPushBack(_consumerCache, consumer);
}

void HilbertStrategy::independenceSplit(auto_ptr<HilbertSlice> slice,
										SliceEvent*& leftEvent,
										auto_ptr<Slice>& leftSlice,
										auto_ptr<Slice>& rightSlice) {
  ASSERT(slice.get() != 0);
  ASSERT(leftEvent == 0);
  ASSERT(leftSlice.get() == 0);
  ASSERT(rightSlice.get() == 0);

  // Construct left event (assignment later).
  auto_ptr<HilbertIndependenceConsumer> consumer = newConsumer();
  consumer->reset(slice->getConsumer(), _indepSplitter, slice->getVarCount());

  // Construct left slice.
  auto_ptr<HilbertSlice> hilbertLeftSlice(newHilbertSlice());
  hilbertLeftSlice->setToProjOf(*slice, consumer->getLeftProjection(),
								consumer->getLeftConsumer());
  leftSlice = hilbertLeftSlice;

  // Construct right slice.
  auto_ptr<HilbertSlice> hilbertRightSlice(newHilbertSlice());
  hilbertRightSlice->setToProjOf(*slice, consumer->getRightProjection(),
								 consumer->getRightConsumer());
  rightSlice = hilbertRightSlice;

  // Deal with slice.
  freeSlice(auto_ptr<Slice>(slice));

  // Done last to avoid memory leak on exception.
  leftEvent = consumer.release();
}

auto_ptr<HilbertIndependenceConsumer> HilbertStrategy::newConsumer() {
  if (_consumerCache.empty())
	return auto_ptr<HilbertIndependenceConsumer>
	  (new HilbertIndependenceConsumer(this));

  auto_ptr<HilbertIndependenceConsumer> consumer(_consumerCache.back());
  _consumerCache.pop_back();
  return consumer;
}
