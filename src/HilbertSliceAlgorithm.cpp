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

void HilbertSliceAlgorithm::content(HilbertSlice& slice) {
  slice.simplify();

  if (!slice.baseCase(_consumer))
	pivotSplit(slice);
}

void HilbertSliceAlgorithm::getPivot(Term& pivot,
									 const HilbertSlice& slice) const {
  const Term& lcm = slice.getLcm();
  size_t varCount = slice.getVarCount();

  pivot.setToIdentity();
  for (size_t var = 0; var < varCount; ++var) {
	if (lcm[var] > 1) {
	  pivot[var] = 1;
	  return;
	}
  }

  ASSERT(false);
}
