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
#include "SliceStrategy.h"

#include "Slice.h"
#include "Term.h"

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

void SliceStrategy::getPivot(Term& pivot, Slice& slice, PivotStrategy ps) {
  ASSERT(ps != Unknown);
  ASSERT(pivot.getVarCount() == slice.getVarCount());
  ASSERT(slice.getIdeal().isMinimallyGenerated());

  if (ps == MinGen) {
	getMinGenPivot(pivot, slice);
	return;
  }

  if (ps == Indep) {
	if (getIndependencePivot(slice, pivot))
	  return;
  }

  size_t var = getBestVar(slice);
  switch (ps) {
  case Minimum:
	pivot.setToIdentity();
	pivot[var] = 1;
	break;

  case Maximum:
	pivot.setToIdentity();
	pivot[var] = slice.getLcm()[var] - 1;
	break;

  case Indep: // Indep uses MidPure as a fall-back.
  case Median:
	pivot.setToIdentity();
	pivot[var] = getMedianPositiveExponentOf(slice, var);
	if (pivot[var] == slice.getLcm()[var])
	  pivot[var] -= 1;
	break;

  case GCD:
	getGCDPivot(slice, pivot, var);
	break;

  default:
	fputs("INTERNAL ERROR: Undefined pivot split strategy.\n", stderr);
	ASSERT(false);
	exit(1);
  }
}

SliceStrategy::PivotStrategy
SliceStrategy::getPivotStrategy(const string& name) {
  if (name == "minimum")
    return Minimum;
  else if (name == "median")
    return Median;
  else if (name == "maximum")
    return Maximum;
  else if (name == "mingen")
    return MinGen;
  else if (name == "indep")
    return Indep;
  else if (name == "gcd")
    return GCD;
  else
	return Unknown;
}
