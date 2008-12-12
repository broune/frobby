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
#include "HilbertBasecase.h"

#include "Ideal.h"
#include "Term.h"

HilbertBasecase::~HilbertBasecase() {
  vector<Ideal*>::iterator end = _idealCache.end();
  for (vector<Ideal*>::iterator it = _idealCache.begin(); it != end; ++it)
	delete *it;
}

void HilbertBasecase::computeCoefficient(Ideal& originalIdeal) {
  ASSERT(_todo.empty());

  _sum = 0;
  size_t varCount = originalIdeal.getVarCount();

  // This object is reused for several different purposes in order to
  // avoid having to allocate and deallocate the underlying data
  // structure.
  _term.reset(varCount);

  Ideal* ideal = &originalIdeal;
  bool negate = false;
  size_t extraSupport = 0;

  while (true) {
	while (true) {
	  // term is used to contain support counts to choose best pivot and
	  // to detect base case.
	  
	  ideal->getSupportCounts(_term);
	  if (_term.getSizeOfSupport() + extraSupport != varCount)
		break;

	  if (_term.isSquareFree()) {
		if ((ideal->getGeneratorCount() % 2) == 1)
		  negate = !negate;
		if (negate)
		  _sum -= 1;
		else
		  _sum += 1;
		break;
	  }

	  if (ideal->getGeneratorCount() == 2) {
		if (negate)
		  _sum -= 1;
		else
		  _sum += 1;
		break;
	  }

	  size_t ridden = eliminate1Counts(*ideal, _term, negate);
	  if (ridden != 0) {
		extraSupport += ridden;
		continue;
	  }

	  if (ideal->getGeneratorCount() == 3) {
		if (negate)
		  _sum -= 2;
		else
		  _sum += 2;
		break;
	  }

	  if (ideal->getGeneratorCount() == 4 &&
		  _term[_term.getFirstMaxExponent()] == 2 &&
		  _term.getSizeOfSupport() == 4) {
		if (negate)
		  _sum += 1;
		else
		  _sum -= 1;
		break;
	  }

	  size_t bestPivotVar = _term.getFirstMaxExponent();

	  // Handle inner slice.
	  Ideal* outer = getNewIdeal();
	  outer->clearAndSetVarCount(varCount);
	  outer->insertNonMultiples(bestPivotVar, 1, *ideal);

	  // inner is subtracted instead of added due to having added the
	  // pivot to the ideal.
	  {
		Entry entry;
		entry.negate = !negate;
		entry.extraSupport = extraSupport + 1;
		entry.ideal = outer;
		_todo.push_back(entry);
	  }

	  // Handle outer slice.
	  ideal->colonReminimize(bestPivotVar, 1);
	  ++extraSupport;

	  // Run loop again to process outer slice.
	}

	if (ideal != &originalIdeal)
	  freeIdeal(ideal);

	if (_todo.empty())
	  break;

	{
	  Entry entry = _todo.back();
	  _todo.pop_back();
	
	  ideal = entry.ideal;
	  negate = entry.negate;
	  extraSupport = entry.extraSupport;
	}
  }

  ASSERT(_todo.empty());
}

const mpz_class& HilbertBasecase::getLastCoefficient() {
  return _sum;
}

void simplify(Ideal& ideal) {
}

bool HilbertBasecase::canSimplify(size_t var,
								  const Ideal& ideal,
								  const Term& counts) {
  if (counts[var] == 0)
	return false;
  Ideal::const_iterator stop = ideal.end();

  size_t varCount = counts.getVarCount();
  for (size_t other = 0; other < varCount; ++other) {
	if (other == var || counts[other] == 0)
	  continue;

	bool can = true;
	for (Ideal::const_iterator it = ideal.begin(); it != stop; ++it) {
	  if ((*it)[var] == 0 && (*it)[other] > 0) {
		can = false;
		break;
	  }
	}
	if (can)
	  return true;
  }
  return false;
}

size_t HilbertBasecase::eliminate1Counts(Ideal& ideal,
										 Term& counts,
										 bool& negate) {
  size_t varCount = ideal.getVarCount();
  size_t adj = 0;
  for (size_t var = 0; var < varCount; ++var) {
	if (counts[var] != 1)
	  continue;

	Ideal::const_iterator it = ideal.getMultiple(var);
	ASSERT(it != ideal.end());

	for (size_t other = 0; other < varCount; ++other) {
	  if ((*it)[other] > 0) {
		++adj;
		if (counts[other] == 1)
		  counts[other] = 0;
	  } else
		counts[other] = 0;
	}

	for (size_t other = 0; other < varCount; ++other) {
	  if (counts[other] > 0) {
		if (!ideal.colonReminimize(other, 1)) {
		  ideal.clear();
		  return 1;
		}
	  }
	}

	it = ideal.getMultiple(var);
	if (it == ideal.end()) {
	  ideal.clear();
	  return 1;
	}
	ideal.remove(it);
	negate = !negate;

	return adj;
  }

  for (size_t var = 0; var < varCount; ++var) {
	if (canSimplify(var, ideal, counts)) {
	  if (!ideal.colonReminimize(var, 1))
		ideal.clear();
	  return adj + 1;
	}
  }

  return adj;
}

Ideal* HilbertBasecase::getNewIdeal() {
  if (_idealCache.empty())
	return new Ideal();

  Ideal* ideal = _idealCache.back();
  _idealCache.pop_back();

  return ideal;
}

void HilbertBasecase::freeIdeal(Ideal* ideal) {
  ASSERT(ideal != 0);

  ideal->clear();
  _idealCache.push_back(ideal);
}
