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
#include "CanonicalTermConsumer.h"
#include "TermTranslator.h"

#include "Term.h"

CanonicalTermConsumer::CanonicalTermConsumer(TermConsumer* consumer,
											 size_t varCount,
											 TermTranslator* translator):
  _consumer(consumer),
  _ideal(varCount),
  _translator(translator) {
}

class TranslatorSorter {
public:
  TranslatorSorter(const TermTranslator* translator):
	_translator(translator) {
  }

  bool operator()(const Exponent* a, const Exponent* b) const {
	return _translator->lessThanReverseLex(a, b);
  }

private:
  const TermTranslator* _translator;
};

CanonicalTermConsumer::~CanonicalTermConsumer() {
  size_t varCount = _ideal.getVarCount();
  Ideal::const_iterator stop = _ideal.end();

  if (_translator == 0)
	_ideal.sortReverseLex();
  else {
	TranslatorSorter sorter(_translator);
	sort(_ideal.begin(), _ideal.end(), sorter);
  }

  Term tmp(varCount);
  for (Ideal::const_iterator it = _ideal.begin(); it != stop; ++it) {
	tmp = *it;
	_consumer->consume(tmp);
  }

  delete _consumer;
}

void CanonicalTermConsumer::consume(const Term& term) {
  ASSERT(term.getVarCount() == _ideal.getVarCount());

  _ideal.insert(term);
}
