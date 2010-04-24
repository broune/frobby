/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "PivotEulerAlg.h"

#include "Ideal.h"
#include "RawSquareFreeTerm.h"
#include "RawSquareFreeIdeal.h"
#include <vector>

namespace Ops = SquareFreeTermOps;

mpz_class rec
(RawSquareFreeIdeal& ideal, Word* eliminated, bool signPlus) {
  Word* lcm = new Word[ideal.getVarCount()](); // todo: too much memory, and leak
  ideal.getLcm(lcm);

  const size_t wordCount = ideal.getWordsPerTerm();
  const size_t varCount = ideal.getVarCount();
  if (Ops::hasFullSupport(eliminated, eliminated + wordCount, varCount)) {
	ASSERT(ideal.getGeneratorCount() == 0);
	return signPlus ? 1 : -1;
  }

  Ops::lcm(lcm, lcm + wordCount, lcm, eliminated);
  if (!Ops::hasFullSupport(lcm, lcm + wordCount, varCount))
	return 0;

  // todo: fix leak
  size_t* counts = new size_t[ideal.getVarCount()]();
  ideal.getVarDividesCounts(counts);
  Word* oneCounts = new Word[ideal.getVarCount()](); // todo: too much memory, and leak
  bool anyOnes = false;
  for (size_t var = 0; var < ideal.getVarCount(); ++var) {
	if (counts[var] == 1) {
	  anyOnes = true;
	  Ops::setExponent(oneCounts, var, true);
	}
  }
  if (anyOnes) {
	Word* gen = ideal.getNotRelativelyPrime(oneCounts);
	ASSERT(gen != 0);
	Ops::assign(oneCounts, oneCounts + wordCount, gen);
	ideal.remove(gen);
	ideal.colonReminimize(oneCounts);
	Ops::lcm(eliminated, eliminated + wordCount, eliminated, oneCounts);
	return rec(ideal, eliminated, !signPlus);
  }

  Word* pivot = new Word[ideal.getWordsPerTerm()]();
  for (size_t var = 0; var < ideal.getVarCount(); ++var) {
	ASSERT(counts[var] != 1);
	if (counts[var] != 0) {
	  Ops::setExponent(pivot, var, true);
	  break;
	}
  }
  ASSERT(!Ops::isIdentity(pivot, pivot + wordCount));

  RawSquareFreeIdeal* outer = newRawSquareFreeIdeal
	(ideal.getVarCount(), ideal.getGeneratorCount());

  ideal.colonReminimize(pivot);
  mpz_class val = rec(ideal, eliminated, signPlus);

  outer->insertReminimize(pivot);
  Ops::lcm(eliminated, eliminated + wordCount, eliminated, pivot);
  delete[] pivot;

  return val + rec(*outer, eliminated, signPlus);
}

PivotEulerAlg::PivotEulerAlg(const Ideal& ideal) {
  Word* eliminated = new Word[ideal.getVarCount()](); // todo: too much memory, and leak

  // todo: fix leak
  RawSquareFreeIdeal* packed = newRawSquareFreeIdeal(ideal.getVarCount(),
													 ideal.getGeneratorCount());

  _euler = rec(*packed, eliminated, true);
}

mpz_class PivotEulerAlg::getEuler() {
  return _euler;
}
