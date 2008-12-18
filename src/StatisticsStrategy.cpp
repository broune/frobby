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
#include "StatisticsStrategy.h"

#include "Slice.h"

StatisticsStrategy::StatisticsStrategy(SliceStrategy* strategy, FILE* out):
  _strategy(strategy),
  _out(out) {
  ASSERT(strategy != 0);
}

StatisticsStrategy::~StatisticsStrategy() {
  delete _strategy; // TODO: replace with auto_ptr
}

void StatisticsStrategy::setUseIndependence(bool use) {
  _strategy->setUseIndependence(use);
}

auto_ptr<Slice> StatisticsStrategy::beginComputing(const Ideal& ideal) {
  return _strategy->beginComputing(ideal);
}

void StatisticsStrategy::doneComputing() {
  _strategy->doneComputing();

  fputs("*** Statistics\n", _out);

  gmp_fprintf(_out, " splits performed: %Zd\n",
			  _splitCount.get_mpz_t());

  if (_splitCount > 0) {
	mpq_class avgGeneratorCount = mpq_class(_generatorCountSum) / _splitCount;
	fprintf(_out, " avg. generator count: %f\n",
			avgGeneratorCount.get_d());

	mpq_class avgVarCount = mpq_class(_varCountSum) / _splitCount;
	fprintf(_out, " avg. variable count: %f\n",
			avgVarCount.get_d());

	mpq_class avgSubtractGeneratorCount =
	  mpq_class(_subtractGeneratorCountSum) / _splitCount;
	fprintf(_out, " avg. subtract generator count: %f\n",
			avgSubtractGeneratorCount.get_d());
  }
}

void StatisticsStrategy::split(auto_ptr<Slice> slice,
							   SliceEvent*& leftEvent,
							   auto_ptr<Slice>& leftSlice,
							   SliceEvent*& rightEvent,
							   auto_ptr<Slice>& rightSlice) {
  ASSERT(slice.get() != 0);

  ++_splitCount;
  _varCountSum += slice->getVarCount();
  _generatorCountSum += slice->getIdeal().getGeneratorCount();
  _subtractGeneratorCountSum += slice->getSubtract().getGeneratorCount();

  _strategy->split(slice, leftEvent, leftSlice, rightEvent, rightSlice);
}

void StatisticsStrategy::freeSlice(auto_ptr<Slice> slice) {
  _strategy->freeSlice(slice);
}
