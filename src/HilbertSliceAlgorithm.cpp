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
  
  size_t varCount = ideal.getVarCount();

  if (!ideal.contains(Term(varCount))) {
	if (ideal.getGeneratorCount() > 0) {
	  Term allOnes(varCount);
	  for (size_t var = 0; var < varCount; ++var)
		allOnes[var] = 1;

	  Ideal temporaryCopy(ideal);
	  temporaryCopy.product(allOnes);
	  HilbertSlice slice(temporaryCopy, Ideal(varCount), Term(varCount));
	  temporaryCopy.clear();

	  content(slice);
	}

	_consumer->consume(1, Term(varCount));
  }

  // Now reset the fields to their default values.
  delete _consumer;
  _consumer = 0;
}

void HilbertSliceAlgorithm::pivotSplit(HilbertSlice& slice) {
  // These scopes are here to preserve memory resources by calling
  // destructors early.
  {
    HilbertSlice inner(slice);

    {
      Term pivot(slice.getVarCount());
      getPivot(pivot, slice);

      ASSERT(!pivot.isIdentity()); 
      ASSERT(!slice.getIdeal().contains(pivot));
      ASSERT(!slice.getSubtract().contains(pivot));

      inner.innerSlice(pivot);
      slice.outerSlice(pivot);
    }

    content(inner);
  }

  // Handle outer slice.
  content(slice);
}

void HilbertSliceAlgorithm::content(HilbertSlice& initialSlice) {
  vector<HilbertSlice*> slices;
  slices.push_back(new HilbertSlice(initialSlice));

  Term pivot(initialSlice.getVarCount());
  size_t todoCount = 1;

  while (todoCount > 0) {
#if 0
	fprintf(stderr, "******************* Slice queue of %i, capacity %i\n",
			(int)todoCount, (int)slices.size());
	for (size_t i = 0; i < todoCount; ++i)
	  slices[i]->print(stderr);
	fputs(          "******************* done with queue\n", stderr);
#endif

	if (todoCount == slices.size())
	  slices.push_back(new HilbertSlice());

	ASSERT(todoCount < slices.size());
	HilbertSlice*& slice = slices[todoCount - 1];
	HilbertSlice*& inner = slices[todoCount];
	--todoCount;

	getPivot(pivot, *slice);

	ASSERT(pivot.getVarCount() == slice->getVarCount());
	ASSERT(!pivot.isIdentity()); 
	ASSERT(!slice->getIdeal().contains(pivot));
	ASSERT(!slice->getSubtract().contains(pivot));

	// Compute inner slice.
	*inner = *slice;
	inner->innerSlice(pivot);
	inner->simplify();
	bool keepInner = !inner->baseCase(_consumer);
	if (!keepInner)
	  inner->clear(); // To preserve memory.

#if 0
	fputs("************* inner\n", stderr);
	inner->print(stderr);
	fprintf(stderr, "KeepInner: %i\n", (int)keepInner);
#endif

	// Compute outer slice.
	slice->outerSlice(pivot);
	slice->simplify();
	bool keepOuter = !slice->baseCase(_consumer);
	if (!keepOuter)
	  slice->clear(); // To preserve memory.

#if 0
	fputs("************* slice\n", stderr);
	slice->print(stderr);
	fprintf(stderr, "KeepOuter: %i\n", (int)keepOuter);
#endif

	if (keepOuter && keepInner) {
	  // Do smallest first to keep queue short.
	  if (inner->getIdeal().getGeneratorCount() >
		  slice->getIdeal().getGeneratorCount())
		swap(inner, slice);
	  todoCount += 2;
	}
	else if (keepOuter && !keepInner)
	  todoCount += 1;
	else if (!keepOuter && keepInner) {
	  swap(inner, slice);
	  todoCount += 1;
	} else {
	  ASSERT(!keepOuter && !keepInner);
	}
  }
}

void HilbertSliceAlgorithm::getPivot(Term& term,
									 HilbertSlice& slice) const {
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
