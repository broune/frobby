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
#include "HilbertSlice.h"

#include "CoefTermConsumer.h"
#include "HilbertBasecase.h"
#include "HilbertStrategy.h"

HilbertSlice::HilbertSlice(HilbertStrategy& strategy):
  Slice(strategy),
  _consumer(0) {
}

HilbertSlice::HilbertSlice(HilbertStrategy& strategy,
                           const Ideal& ideal, const Ideal& subtract,
                           const Term& multiply, CoefTermConsumer* consumer):
  Slice(strategy, ideal, subtract, multiply),
  _consumer(consumer) {
  ASSERT(consumer != 0);
}

bool HilbertSlice::baseCase(bool simplified) {
  ASSERT(_consumer != 0);

  // Check that each variable appears in some minimal generator.
  if (getLcm().getSizeOfSupport() < _varCount)
    return true;

  if (!getLcm().isSquareFree())
    return false;

  if (_varCount == 0)
    return true;

  // TODO: find a way other than static to use the same basecase
  // object every time, instead of allocating a new one. This provides
  // around a 4% speed-up, at least on Cygwin. We cannot use static
  // since the base case has an mpz_class, and static mpz_class
  // crashes on Mac OS X.
  HilbertBasecase basecase;
  basecase.computeCoefficient(_ideal);
  const mpz_class& coef = basecase.getLastCoefficient();

  if (coef != 0)
    _consumer->consume(coef, getMultiply());
  clearIdealAndSubtract();
  return true;
}

Slice& HilbertSlice::operator=(const Slice& slice) {
  ASSERT(dynamic_cast<const HilbertSlice*>(&slice) != 0);

  Slice::operator=(slice);
  _consumer = ((HilbertSlice&)slice)._consumer;
  return *this;
}

bool HilbertSlice::simplifyStep() {
  if (applyLowerBound())
    return true;

  pruneSubtract();
  return false;
}

void HilbertSlice::setToProjOf(const Slice& slice,
                               const Projection& projection,
                               CoefTermConsumer* consumer) {
  ASSERT(consumer != 0);

  Slice::setToProjOf(slice, projection);
  _consumer = consumer;
}

void HilbertSlice::swap(HilbertSlice& slice) {
  Slice::swap(slice);
  std::swap(_consumer, slice._consumer);
}

bool HilbertSlice::getLowerBound(Term& bound, size_t var) const {
  bool seenAny = false;

  Ideal::const_iterator stop = getIdeal().end();
  for (Ideal::const_iterator it = getIdeal().begin(); it != stop; ++it) {
    if ((*it)[var] == 0)
      continue;

    if (seenAny)
      bound.gcd(bound, *it);
    else {
      bound = *it;
      seenAny = true;
    }
  }

  if (seenAny) {
    ASSERT(bound[var] >= 1);
    bound.decrement();
    return true;
  } else {
    // In this case the content is empty.
    return false;
  }
}
