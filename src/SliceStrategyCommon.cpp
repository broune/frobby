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
#include "SliceStrategyCommon.h"

#include "Slice.h"

SliceStrategyCommon::SliceStrategyCommon(const SplitStrategy* splitStrategy):
  _split(splitStrategy),
  _useIndependence(true) {
  ASSERT(splitStrategy != 0);
}

SliceStrategyCommon::~SliceStrategyCommon() {
  while (!_sliceCache.empty()) {
	delete _sliceCache.back();
	_sliceCache.pop_back();
  }
}

void SliceStrategyCommon::freeSlice(Slice* slice) {
  ASSERT(slice != 0);
  ASSERT(debugIsValidSlice(slice));

  slice->clearIdealAndSubtract(); // To preserve memory.
  _sliceCache.push_back(slice);
}

void SliceStrategyCommon::setUseIndependence(bool use) {
  _useIndependence = use;
}

Slice* SliceStrategyCommon::newSlice() {
  Slice* slice;
  if (!_sliceCache.empty()) {
	slice = _sliceCache.back();
	_sliceCache.pop_back();
  } else
	slice = allocateSlice();

  ASSERT(debugIsValidSlice(slice));
  return slice;
}

void SliceStrategyCommon::pivotSplit(Slice* slice,
									 Slice*& leftSlice,
									 Slice*& rightSlice) {
  _pivotTmp.reset(slice->getVarCount());
  getPivot(_pivotTmp, *slice);

  // Assert valid pivot.
  ASSERT(_pivotTmp.getVarCount() == slice->getVarCount());
  ASSERT(!_pivotTmp.isIdentity()); 
  ASSERT(!slice->getIdeal().contains(_pivotTmp));
  ASSERT(!slice->getSubtract().contains(_pivotTmp));

  // The inner slice.
  leftSlice = newSlice();
  *leftSlice = *slice;
  leftSlice->innerSlice(_pivotTmp);
  leftSlice->simplify();

  // The outer slice
  rightSlice = slice;
  rightSlice->outerSlice(_pivotTmp);
  rightSlice->simplify();

  // Process the smaller one first to preserve memory.
  if (leftSlice->getIdeal().getGeneratorCount() <
	  rightSlice->getIdeal().getGeneratorCount())
	std::swap(leftSlice, rightSlice);
}

bool SliceStrategyCommon::getUseIndependence() const {
  return _useIndependence;
}
