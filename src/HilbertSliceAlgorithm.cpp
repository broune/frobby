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
#include "HilbertSliceAlgorithm.h"

#include "HilbertSlice.h"
#include "TermConsumer.h"

#include "Term.h"
#include "VarNames.h"
#include "IOHandler.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "Partition.h"
#include "HilbertStrategy.h"

#include "IndependenceSplitter.h"

#include "CoefTermConsumer.h"

HilbertSliceAlgorithm::HilbertSliceAlgorithm():
  _consumer(0) {
}

void HilbertSliceAlgorithm::setConsumer(CoefTermConsumer* consumer) {
  delete _consumer;
  _consumer = consumer;
}

void HilbertSliceAlgorithm::run(const Ideal& ideal) {
  ASSERT(_consumer != 0);

  HilbertStrategy* strategy = new HilbertStrategy();

  vector<HilbertSlice*> slices;
  HilbertSlice* initialSlice = strategy->setupInitialSlice(ideal, _consumer);
  if (!initialSlice->baseCase())
	slices.push_back(initialSlice);

  while (!slices.empty()) {
	HilbertSlice* slice = slices.back();
	slices.pop_back();

	ASSERT(!slice->baseCase());

	pair<HilbertSlice*, HilbertSlice*> slicePair = strategy->split(slice);

	if (slicePair.first->baseCase())
	  strategy->freeSlice(slicePair.first);
	else
	  slices.push_back(slicePair.first);

	if (slicePair.second->baseCase())
	  strategy->freeSlice(slicePair.second);
	else
	  slices.push_back(slicePair.second);
  }

  delete strategy;

  // Now reset the fields to their default values.
  delete _consumer;
  _consumer = 0;
}
