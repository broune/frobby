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
#include "TranslatingTermConsumer.h"

#include "Term.h"
#include "TermTranslator.h"
#include "BigTermConsumer.h"

TranslatingTermConsumer::TranslatingTermConsumer
(BigTermConsumer* consumer, TermTranslator* translator):
  _consumer(consumer),
  _translator(translator) {
  ASSERT(consumer != 0);
  ASSERT(translator != 0);
}

void TranslatingTermConsumer::beginConsuming() {
  _consumer->beginConsuming(_translator->getNames());
}

void TranslatingTermConsumer::consume(const Term& term) {
  ASSERT(term.getVarCount() == _translator->getVarCount());

  _consumer->consume(term, *_translator);
}

void TranslatingTermConsumer::doneConsuming() {
  _consumer->doneConsuming();
}
