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
TotalDegreeCoefTermConsumer(auto_ptr<CoefBigTermConsumer> consumer,
							const TermTranslator& translator):
  _consumer(*consumer),
  _consumerOwner(consumer),
  _translator(translator) {
  ASSERT(_consumerOwner.get() != 0);
}

TotalDegreeCoefTermConsumer::
TotalDegreeCoefTermConsumer(CoefBigTermConsumer& consumer,
							const TermTranslator& translator):
  _consumer(consumer),
  _translator(translator) {
}

void TotalDegreeCoefTermConsumer::consumeRing(const VarNames& names) {
  // Do nothing since taking the total degree discards the original
  // ring.
}

void TotalDegreeCoefTermConsumer::beginConsuming() {
}

void TotalDegreeCoefTermConsumer::consume(const mpz_class& coef,
										  const Term& term) {
  ASSERT(term.getVarCount() == _translator.getVarCount());
  if (coef == 0)
	return;

  // Compute total degree using _tmp.
  _tmp = 0;
  for (size_t var = 0; var < term.getVarCount(); ++var)
	_tmp += _translator.getExponent(var, term);

  pair<map<mpz_class, mpz_class>::iterator, bool> insertionResult =
	_polynomial.insert(make_pair(_tmp, coef));
  map<mpz_class, mpz_class>::iterator entry = insertionResult.first;

  if (!insertionResult.second) {
	entry->second += coef;
	if (entry->second == 0)
	  _polynomial.erase(entry);
  }
}

void TotalDegreeCoefTermConsumer::doneConsuming() {
  VarNames names;
  names.addVar("t");
  _consumer.consumeRing(names);

  _consumer.beginConsuming();
  vector<mpz_class> term(1);

  // Reverse direction to get canonical reverse lex order.
  map<mpz_class, mpz_class>::reverse_iterator stop = _polynomial.rend();
  for (map<mpz_class, mpz_class>::reverse_iterator it = _polynomial.rbegin();
	   it != stop; ++it) {
	ASSERT(it->second != 0);
	term[0] = it->first;
	_consumer.consume(it->second, term);
  }

  _consumer.doneConsuming();
}
