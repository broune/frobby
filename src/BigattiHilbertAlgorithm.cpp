/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
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
#include "BigattiHilbertAlgorithm.h"

#include "Ideal.h"
#include "CoefTermConsumer.h"

BigattiHilbertAlgorithm::BigattiHilbertAlgorithm(CoefTermConsumer* consumer):
	_consumer(consumer) {
}

void BigattiHilbertAlgorithm::run(const Ideal& ideal) {
  ASSERT(ideal.isMinimallyGenerated());
  _varCount = ideal.getVarCount();
  _output.clearAndSetVarCount(_varCount);
  Term term(_varCount);
  hilbert(ideal, term);

  _output.sortTermsReverseLex(true);
  _consumer->consume(_output);
}

void BigattiHilbertAlgorithm::hilbert(const Ideal& ideal, const Term& term) {
  if (isBaseCase(ideal)) {
    basecase(ideal.begin(), ideal.end(), true, term);
    return;
  }

  Term pivot(_varCount);
  getPivot(ideal, pivot);

  Ideal colon = ideal;
  colon.colonReminimize(pivot);
  Term product(_varCount);
  product.product(pivot, term);
  hilbert(colon, product);

  Ideal add = ideal;
  add.insertReminimize(pivot);
  hilbert(add, term);
}

bool BigattiHilbertAlgorithm::isBaseCase(const Ideal& ideal) {
	return ideal.disjointSupport();
}

void BigattiHilbertAlgorithm::basecase(Ideal::const_iterator begin, Ideal::const_iterator end, bool plus, const Term& term) {
  if (begin == end)
    _output.add(plus ? 1 : -1, term);
  else {
    basecase(begin + 1, end, plus, term);
    Term product(_varCount);
    product.product(term, *begin);
    basecase(begin + 1, end, !plus, product);
  }
}

void BigattiHilbertAlgorithm::getPivot(const Ideal& ideal, Term& pivot) {
  Term counts(_varCount);
  ideal.getSupportCounts(counts);
  size_t var = counts.getFirstMaxExponent();

  pivot.setToIdentity();
  pivot[var] = 1;
}
