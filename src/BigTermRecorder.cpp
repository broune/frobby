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
#include "BigTermRecorder.h"

#include "BigIdeal.h"
#include "Term.h"
#include "TermTranslator.h"

BigTermRecorder::BigTermRecorder(BigIdeal* recordInto):
  _recordInto(recordInto) {
  ASSERT(recordInto != 0);
}

void BigTermRecorder::beginConsuming() {
}

void BigTermRecorder::consume(const Term& term, TermTranslator* translator) {
  ASSERT(term.getVarCount() == _recordInto->getVarCount());
  ASSERT(translator->getVarCount() == _recordInto->getVarCount());

  size_t varCount = _recordInto->getVarCount();
  _recordInto->newLastTerm();
  for (size_t var = 0; var < varCount; ++var)
	_recordInto->getLastTermExponentRef(var) =
	  translator->getExponent(var, term);
}

void BigTermRecorder::consume(mpz_ptr* term) {
  size_t varCount = _recordInto->getVarCount();
  _recordInto->newLastTerm();
  for (size_t var = 0; var < varCount; ++var)
	_recordInto->getLastTermExponentRef(var) = mpz_class(term[var]);
}

void BigTermRecorder::doneConsuming() {
}
