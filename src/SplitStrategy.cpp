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
#include "SplitStrategy.h"

#include "Slice.h"


bool getIndependencePivot(const Slice& slice, Term& pivot) {
  if (slice.getVarCount() == 1)
	return false;

  for (int attempts = 0; attempts < 10; ++attempts) {
	// Pick two distinct variables.
	size_t var1 = rand() % slice.getVarCount();
	size_t var2 = rand() % (slice.getVarCount() - 1);
	if (var2 >= var1)
	  ++var2;

	// Make pivot as big as it can be while making var1 and var2
	// independent of each other.
	bool first = true;
	Ideal::const_iterator end = slice.getIdeal().end();
	for (Ideal::const_iterator it = slice.getIdeal().begin();
		 it != end; ++it) {
	  if ((*it)[var1] == 0 || (*it)[var2] == 0)
		continue;

	  if (first)
		pivot = *it;
	  else {
		for (size_t var = 0; var < slice.getVarCount(); ++var)
		  if (pivot[var] >= (*it)[var])
			pivot[var] = (*it)[var] - 1;
	  }
	}

	if (!first && !pivot.isIdentity())
	  return true;
  }

  return false;
}

size_t getRandomSupportVar(const Term& term) {
  ASSERT(!term.isIdentity());

  size_t selected = rand() % term.getSizeOfSupport();
  for (size_t var = 0; ; ++var) {
	ASSERT(var < term.getVarCount());
	if (term[var] == 0)
	  continue;

	if (selected == 0)
	  return var;
	--selected;
  }
}

void getGCDPivot(const Slice& slice, Term& pivot, size_t var) {
  size_t nonDivisibleCount = 0;
  Ideal::const_iterator end = slice.getIdeal().end();
  for (Ideal::const_iterator it = slice.getIdeal().begin();
	   it != end; ++it)
	if ((*it)[var] >= 2)
	  ++nonDivisibleCount;
	
  for (int i = 0; i < 3; ++i) {
	size_t selected = rand() % nonDivisibleCount;
	for (Ideal::const_iterator it = slice.getIdeal().begin(); ; ++it) {
	  ASSERT(it != end);
	  if ((*it)[var] < 2)
		continue;

	  if (selected == 0) {
		if (i == 0)
		  pivot = *it;
		else
		  pivot.gcd(pivot, *it);
		break;
	  }
	  --selected;
	}
  }

  pivot.decrement();
}

void getMinGenPivot(Term& pivot, const Slice& slice) {
  size_t nonSquareFreeCount = 0;
  Ideal::const_iterator end = slice.getIdeal().end();
  for (Ideal::const_iterator it = slice.getIdeal().begin();
	   it != end; ++it)
	if (!::isSquareFree(*it, slice.getVarCount()))
	  ++nonSquareFreeCount;

  size_t selected = rand() % nonSquareFreeCount;
  for (Ideal::const_iterator it = slice.getIdeal().begin(); ; ++it) {
	ASSERT(it != end);
	if (::isSquareFree(*it, slice.getVarCount()))
	  continue;

	if (selected == 0) {
	  pivot = *it;
	  break;
	}
	--selected;
  }

  pivot.decrement();
}

Exponent getMedianPositiveExponentOf(Slice& slice, size_t var) {
  slice.singleDegreeSortIdeal(var);
  Ideal::const_iterator end = slice.getIdeal().end();
  Ideal::const_iterator begin = slice.getIdeal().begin();
  while ((*begin)[var] == 0) {
	++begin;
	ASSERT(begin != end);
  }
  return (*(begin + (distance(begin, end) ) / 2))[var];
}

// Returns the variable that divides the most minimal generators of
// those where some minimal generator is divisible by the square of
// that variable.
size_t getBestVar(const Slice& slice) {
  Term co(slice.getVarCount());
  slice.getIdeal().getSupportCounts(co);

  const Term& lcm = slice.getLcm();
  for (size_t var = 0; var < slice.getVarCount(); ++var)
	if (lcm[var] <= 1)
	  co[var] = 0;

  ASSERT(!co.isIdentity());
	
  Exponent maxCount = co[co.getFirstMaxExponent()];
  for (size_t var = 0; var < slice.getVarCount(); ++var)
	if (co[var] < maxCount)
	  co[var] = 0;

  // Choose a deterministically random variable among those that are
  // best. This helps to avoid getting into a bad pattern.
  return getRandomSupportVar(co);
}

void SplitStrategy::getPivot
(Term& pivot, Slice& slice, SplitStrategy psParam) {
  SplitType ps = psParam._id;
  ASSERT(ps != SplitStrategy::Unknown);
  ASSERT(pivot.getVarCount() == slice.getVarCount());
  ASSERT(slice.getIdeal().isMinimallyGenerated());

  if (ps == SplitStrategy::MinGen) {
	getMinGenPivot(pivot, slice);
	return;
  }

  if (ps == SplitStrategy::Indep) {
	if (getIndependencePivot(slice, pivot))
	  return;
  }

  size_t var = getBestVar(slice);
  switch (ps) {
  case SplitStrategy::Minimum:
	pivot.setToIdentity();
	pivot[var] = 1;
	break;

  case SplitStrategy::Maximum:
	pivot.setToIdentity();
	pivot[var] = slice.getLcm()[var] - 1;
	break;

  case SplitStrategy::Indep: // Indep uses MidPure as a fall-back.
  case SplitStrategy::Median:
	pivot.setToIdentity();
	pivot[var] = getMedianPositiveExponentOf(slice, var);
	if (pivot[var] == slice.getLcm()[var])
	  pivot[var] -= 1;
	break;

  case SplitStrategy::GCD:
	getGCDPivot(slice, pivot, var);
	break;

  default:
	fputs("INTERNAL ERROR: Undefined pivot split strategy.\n", stderr);
	ASSERT(false);
	exit(1);
  }
}

size_t SplitStrategy::getLabelSplitVariable(const Slice& slice,
											SplitStrategy ls) {
  SplitType labelStrategy = ls._id;
  Term co(slice.getVarCount());
  slice.getIdeal().getSupportCounts(co);

  if (labelStrategy == MaxLabel) {
	// Return the variable that divides the most minimal generators.
	// This cannot be an invalid split because if every count was 1,
	// then this would be a base case.
	return co.getFirstMaxExponent();
  }

  // For each variable, count number of terms with exponent of 1
  Term co1(slice.getVarCount());
  Ideal::const_iterator end = slice.getIdeal().end();
  for (Ideal::const_iterator it = slice.getIdeal().begin();
	   it != end; ++it) {
	// This way we avoid bad splits.
	if (getSizeOfSupport(*it, slice.getVarCount()) == 1)
	  continue;
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  if ((*it)[var] == 1)
		co1[var] += 1;
  }

  // The slice is simplified and not a base case slice.
  ASSERT(!co1.isIdentity());

  if (labelStrategy == VarLabel) {
	// Return the least variable that is valid.
	for (size_t var = 0; ; ++var) {
	  ASSERT(var < slice.getVarCount());
	  if (co1[var] > 0)
		return var;
	}
  }

  if (labelStrategy != MinLabel) {
	fputs("INTERNAL ERROR: Undefined label split type.\n", stderr);
	ASSERT(false);
	exit(1);
  }

  // Zero those variables of co that have more than the least number
  // of exponent 1 minimal generators.
  size_t mostGeneric = 0;
  for (size_t var = 1; var < slice.getVarCount(); ++var)
	if (mostGeneric == 0 || (mostGeneric > co1[var] && co1[var] > 0))
	  mostGeneric = co1[var];
  for (size_t var = 0; var < slice.getVarCount(); ++var)
	if (co1[var] != mostGeneric)
	  co[var] = 0;

  // Among those with least exponent 1 minimal generators, return
  // the variable that divides the most minimal generators.
  return co.getFirstMaxExponent();
}
