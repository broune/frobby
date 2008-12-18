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
#ifndef STATISTICS_STRATEGY
#define STATISTICS_STRATEGY

#include "SliceStrategy.h"

// A wrapper for a slice that collects statistics on what is going on,
// while delegating everything to the strategy being wrapped.
class StatisticsStrategy : public SliceStrategy {
 public:
  // Takes over ownership of strategy. Does not close out in
  // destructor.
  StatisticsStrategy(SliceStrategy* strategy, FILE* out);
  virtual ~StatisticsStrategy();

  virtual void setUseIndependence(bool use);

  virtual auto_ptr<Slice> beginComputing(const Ideal& ideal);
  virtual void doneComputing();

  virtual void split(auto_ptr<Slice> slice,
					 SliceEvent*& leftEvent, auto_ptr<Slice>& leftSlice,
					 SliceEvent*& rightEvent, auto_ptr<Slice>& rightSlice);

  virtual void freeSlice(auto_ptr<Slice> slice);

 private:
  SliceStrategy* _strategy;
  FILE* _out;

  mpz_class _splitCount;
  mpz_class _generatorCountSum;
  mpz_class _varCountSum;
  mpz_class _subtractGeneratorCountSum;
};

#endif
