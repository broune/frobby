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
#include "CanonicalCoefTermConsumer.h"

#include "Term.h"

CanonicalCoefTermConsumer::
CanonicalCoefTermConsumer(CoefTermConsumer* consumer, size_t varCount):
  _consumer(consumer),
  _polynomial(varCount) {
  ASSERT(consumer != 0);
}

CanonicalCoefTermConsumer::~CanonicalCoefTermConsumer() {
  _polynomial.sortTermsLex();
  for (size_t index = 0; index < _polynomial.getTermCount(); ++index)
	_consumer->consume(_polynomial.getCoef(index), _polynomial.getTerm(index));

  delete _consumer;
}

void CanonicalCoefTermConsumer::
consume(const mpz_class& coef, const Term& term) {
  ASSERT(term.getVarCount() == _polynomial.getVarCount());

  _polynomial.add(coef, term);
}
