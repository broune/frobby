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
#include "CoefBigTermConsumer.h"

#include "BigPolynomial.h"
#include "Term.h"

// Do not use this implementation when in need of high performance, since
// allocating a new big term each time is not very efficient.
void CoefBigTermConsumer::consume(const mpz_class& coef, const Term& term) {
  vector<mpz_class> bigTerm;
  bigTerm.reserve(term.getVarCount());
  for (size_t var = 0; var < term.getVarCount(); ++var)
    bigTerm.push_back(term[var]);
  consume(coef, bigTerm);
}

void CoefBigTermConsumer::consume(const BigPolynomial& poly) {
  consumeRing(poly.getNames());
  beginConsuming();
  for (size_t index = 0; index < poly.getTermCount(); ++index)
    consume(poly.getCoef(index), poly.getTerm(index));
  doneConsuming();
}
