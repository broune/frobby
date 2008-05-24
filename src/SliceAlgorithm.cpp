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
#include "SliceAlgorithm.h"

#include "Slice.h"
#include "MsmStrategy.h"
#include "HilbertStrategy.h"
#include "SliceEvent.h"

void runSliceAlgorithm(Slice* initialSlice, SliceStrategy* strategy) {
  ASSERT(initialSlice != 0);
  ASSERT(strategy != 0);

  vector<SliceEvent*> events;
  vector<Slice*> slices;
  slices.push_back(initialSlice);

  while (!slices.empty()) {
	Slice* slice = slices.back();
	slices.pop_back();

	if (slice == 0) {
	  events.back()->raiseEvent();
	  events.pop_back();
	  continue;
	}

	if (slice->baseCase()) {
	  strategy->freeSlice(slice);
	  continue;
	}

	SliceEvent* leftEvent = 0;
	SliceEvent* rightEvent = 0;
	Slice* leftSlice = 0;
	Slice* rightSlice = 0;
	strategy->split(slice,
					leftEvent, leftSlice,
					rightEvent, rightSlice);

	if (leftEvent != 0) {
	  slices.push_back(0);
	  events.push_back(leftEvent);
	}

	if (leftSlice != 0)
	  slices.push_back(leftSlice);

	if (rightEvent != 0) {
	  slices.push_back(0);
	  events.push_back(rightEvent);
	}

	if (rightSlice != 0)
	  slices.push_back(rightSlice);
  }
}


void computeMaximalStandardMonomials(Ideal& ideal, MsmStrategy* strategy) {
  runSliceAlgorithm(strategy->setupInitialSlice(ideal, strategy), strategy);
}

void computeHilbertSeries(const Ideal& ideal, CoefTermConsumer* consumer) {
  HilbertStrategy strategy;
  runSliceAlgorithm(strategy.setupInitialSlice(ideal, consumer), &strategy);
}

bool computeSingleMSM2(const Slice& slice, Term& msm) {
  msm.reset(slice.getVarCount());
  Term lcm(slice.getLcm());

  for (size_t var = 0; var < msm.getVarCount(); ++var) {
    msm[var] = lcm[var];
    Ideal::const_iterator it = slice.getIdeal().begin();
    Ideal::const_iterator end = slice.getIdeal().end();
    for (; it != end; ++it) {
	  if (msm.dominates(*it)) {
		ASSERT((*it)[var] > 0);
		msm[var] = (*it)[var] - 1;
      }
    }
  }
  
#ifdef DEBUG
  ASSERT(!slice.getIdeal().contains(msm));
  for (size_t var = 0; var < msm.getVarCount(); ++var) {
    msm[var] += 1;
    ASSERT(slice.getIdeal().contains(msm));
    msm[var] -= 1;
  }
#endif

  if (slice.getSubtract().contains(msm))
    return false;

  msm.product(msm, slice.getMultiply());
  return true;
}

bool computeSingleMSM(const Slice& slice, Term& msm) {
  for (size_t var = 0; var < msm.getVarCount(); ++var)
    if (slice.getLcm()[var] == 0)
      return false;

  // Extract terms into a container we can alter without changing
  // slice.
  vector<const Exponent*> terms;
  {
    terms.reserve(slice.getIdeal().getGeneratorCount());
    Ideal::const_iterator it = slice.getIdeal().begin();
    Ideal::const_iterator end = slice.getIdeal().end();
    for (; it != end; ++it)
      terms.push_back(*it);
  }

  msm.reset(slice.getVarCount());

  // do offset 0 to avoid negative vars below
  msm[0] = slice.getLcm()[0] - 1;
  {
    // Do things from the right so that the call do dominates below
    // will exit earlier.
    for (size_t var = msm.getVarCount() - 1; var >= 1; --var) {
      msm[var] = slice.getLcm()[var];

      bool foundLabel = false;
      vector<const Exponent*>::iterator it = terms.begin();
      vector<const Exponent*>::iterator end = terms.end();
      for (; it != end; ++it) {
		if (msm.dominates(*it)) {
		  ASSERT((*it)[var] > 0);
		  msm[var] = (*it)[var] - 1;
		  foundLabel = true;
		}
      }
      if (!foundLabel)
		return false;

      // Remove irrelevant terms
      it = terms.begin();
      for (; it != terms.end();) {
		if ((*it)[var] <= msm[var])
		  ++it;
		else {
		  if (it + 1 != terms.end())
			*it = terms.back();
		  terms.pop_back();
		}
      }
    }
  }

#ifdef DEBUG
  ASSERT(!slice.getIdeal().contains(msm));
  for (size_t var = 0; var < msm.getVarCount(); ++var) {
    msm[var] += 1;
    ASSERT(slice.getIdeal().contains(msm));
    msm[var] -= 1;
  }
#endif

  if (slice.getSubtract().contains(msm))
    return false;

  msm.product(msm, slice.getMultiply());
  return true;
}
