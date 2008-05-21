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

HilbertStrategy::~HilbertStrategy() {
  while (!_sliceCache.empty()) {
	delete _sliceCache.back();
	_sliceCache.pop_back();
  }
}

HilbertSlice* HilbertStrategy::setupInitialSlice(const Ideal& ideal,
												 CoefTermConsumer* consumer) {
  ASSERT(consumer != 0);
  _term.reset(ideal.getVarCount());

  size_t varCount = ideal.getVarCount();
  Ideal sliceIdeal(varCount);

  if (!ideal.contains(Term(varCount))) {
	consumer->consume(1, Term(varCount));

	if (ideal.getGeneratorCount() > 0) {
	  Term allOnes(varCount);
	  for (size_t var = 0; var < varCount; ++var)
		allOnes[var] = 1;

	  sliceIdeal = ideal;
	  sliceIdeal.product(allOnes);
	}
  }

  HilbertSlice* slice = new HilbertSlice(sliceIdeal, Ideal(varCount),
										 Term(varCount), consumer);
  slice->simplify();
  return slice;
}

pair<HilbertSlice*, HilbertSlice*>
HilbertStrategy::split(HilbertSlice* slice) {
  Term _term(slice->getVarCount());
  getPivot(_term, *slice);

  ASSERT(_term.getVarCount() == slice->getVarCount());
  ASSERT(!_term.isIdentity()); 
  ASSERT(!slice->getIdeal().contains(_term));
  ASSERT(!slice->getSubtract().contains(_term));

  // Compute inner slice.
  HilbertSlice* inner = newSlice();
  *inner = *slice;
  inner->innerSlice(_term);
  inner->simplify();

  // Compute outer slice.
  slice->outerSlice(_term);
  slice->simplify();

  if (inner->getIdeal().getGeneratorCount() <
	  slice->getIdeal().getGeneratorCount())
	return make_pair(inner, slice);
  else
	return make_pair(slice, inner);
}

void HilbertStrategy::freeSlice(HilbertSlice* slice) {
  ASSERT(slice != 0);

  slice->clear(); // To preserve memory.
  _sliceCache.push_back(slice);
}

HilbertSlice* HilbertStrategy::newSlice() {
  if (_sliceCache.empty())
	return new HilbertSlice();

  HilbertSlice* slice = _sliceCache.back();
  _sliceCache.pop_back();
  return slice;
}

void HilbertStrategy::getPivot(Term& term, HilbertSlice& slice) {
  ASSERT(term.getVarCount() == slice.getVarCount());

  // Get best (most populated) variable to split on. Term serves
  // double-duty as a container of the support counts.
  slice.getIdeal().getSupportCounts(term);

  const Term& lcm = slice.getLcm();
  for (size_t var = 0; var < slice.getVarCount(); ++var)
	if (lcm[var] <= 1)
	  term[var] = 0;

  ASSERT(!term.isIdentity());
  size_t bestVar = term.getFirstMaxExponent();

  // Get median positive exponent of bestVar.
  slice.singleDegreeSortIdeal(bestVar);
  Ideal::const_iterator end = slice.getIdeal().end();
  Ideal::const_iterator begin = slice.getIdeal().begin();
  while ((*begin)[bestVar] == 0) {
	++begin;
	ASSERT(begin != end);
  }
  term.setToIdentity();
  term[bestVar] = (*(begin + (distance(begin, end) ) / 2))[bestVar];
  if (term[bestVar] == lcm[bestVar])
	term[bestVar] -= 1;

  ASSERT(!term.isIdentity());
  ASSERT(!slice.getIdeal().contains(term));
  ASSERT(!slice.getSubtract().contains(term));
  ASSERT(term.strictlyDivides(slice.getLcm()));
}
