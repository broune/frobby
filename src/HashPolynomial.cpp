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
#include "HashPolynomial.h"

#include "CoefTermConsumer.h"

HashPolynomial::HashPolynomial(size_t varCount):
  _varCount(varCount) {
}

void HashPolynomial::clearAndSetVarCount(size_t varCount) {
  _terms.clear();
  _varCount = varCount;
}

void HashPolynomial::add(const mpz_class& coef, const Term& term) {
  ASSERT(_varCount == term.getVarCount());

  if (coef == 0)
	return;

  map<Term, mpz_class>::iterator pos = _terms.lower_bound(term);
  if (pos != _terms.end() && pos->first == term) {
	pos->second += coef;
	if (pos->second == 0)
	  _terms.erase(pos);
  } else
	_terms.insert(pos, make_pair(term, coef));
}

void HashPolynomial::feedTo(CoefTermConsumer& consumer) const {
  consumer.beginConsuming();

  typedef map<Term, mpz_class>::const_reverse_iterator iter;
  iter end = _terms.rend();
  for (iter it = _terms.rbegin(); it != end; ++it)
	consumer.consume(it->second, it->first);

  consumer.doneConsuming();
}
