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
#include "TotalDegreeCoefTermConsumer.h"

#include "CoefBigTermConsumer.h"
#include "TermTranslator.h"
#include "Term.h"

TotalDegreeCoefTermConsumer::
TotalDegreeCoefTermConsumer(CoefBigTermConsumer* consumer,
							TermTranslator* translator):
  _consumer(consumer),
  _translator(translator),
  _tmp(0) {
  ASSERT(consumer != 0);
  ASSERT(_translator != 0);
  ASSERT(_tmp == 0); // This is an invariant.
}

TotalDegreeCoefTermConsumer::~TotalDegreeCoefTermConsumer() {
  map<mpz_class, mpz_class>::iterator stop = _polynomial.end();
  for (map<mpz_class, mpz_class>::iterator it = _polynomial.begin();
	   it != stop; ++it) {
	ASSERT(it->second != 0);
	mpz_ptr ptr = (mpz_ptr)it->first.get_mpz_t();
	_consumer->consume(it->second, &ptr);
  }
  delete _consumer;
}

void TotalDegreeCoefTermConsumer::consume(const mpz_class& coef,
										  const Term& term) {
  ASSERT(term.getVarCount() == _translator->getNames().getVarCount());
  ASSERT(coef != 0);
  ASSERT(_tmp == 0);

  // Compute total degree using _tmp.
  _tmp = 0;
  for (size_t var = 0; var < term.getVarCount(); ++var)
	_tmp += _translator->getExponent(var, term);

  pair<map<mpz_class, mpz_class>::iterator, bool> insertionResult =
	_polynomial.insert(make_pair(_tmp, coef));
  map<mpz_class, mpz_class>::iterator entry = insertionResult.first;

  _tmp = 0;

  if (!insertionResult.second) {
	entry->second += coef;
	if (entry->second == 0)
	  _polynomial.erase(entry);
  }

  ASSERT(_tmp == 0);
}
