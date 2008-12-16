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
#include "TranslatingCoefTermConsumer.h"

#include "CoefBigTermConsumer.h"
#include "Term.h"
#include "TermTranslator.h"

TranslatingCoefTermConsumer::TranslatingCoefTermConsumer
(CoefBigTermConsumer* consumer, TermTranslator* translator):
  _consumer(consumer),
  _translator(translator) {
  ASSERT(consumer != 0);
  ASSERT(translator != 0);
}

void TranslatingCoefTermConsumer::beginConsuming() {
  _consumer->beginConsuming();
}

void TranslatingCoefTermConsumer::
consume(const mpz_class& coef, const Term& term) {
  ASSERT(term.getVarCount() == _translator->getVarCount());

  _consumer->consume(coef, term, _translator);
}

void TranslatingCoefTermConsumer::doneConsuming() {
  _consumer->doneConsuming();
}
