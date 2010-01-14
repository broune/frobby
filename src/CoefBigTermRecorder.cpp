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
#include "CoefBigTermRecorder.h"

#include "BigPolynomial.h"

CoefBigTermRecorder::CoefBigTermRecorder(BigPolynomial* recordInto):
  _recordInto(recordInto) {
  ASSERT(recordInto != 0);
}

void CoefBigTermRecorder::consumeRing(const VarNames& names) {
  _recordInto->clearAndSetNames(names);
}

void CoefBigTermRecorder::beginConsuming() {
}

void CoefBigTermRecorder::consume(const mpz_class& coef,
                                  const Term& term,
                                  const TermTranslator& translator) {
  _recordInto->add(coef, term, translator);
}

void CoefBigTermRecorder::consume(const mpz_class& coef,
                                  const vector<mpz_class>& term) {
  _recordInto->add(coef, term);
}

void CoefBigTermRecorder::doneConsuming() {
}
