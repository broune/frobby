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
#ifndef TRANSLATING_TERM_CONSUMER_GUARD
#define TRANSLATING_TERM_CONSUMER_GUARD

#include "TermConsumer.h"

class BigTermConsumer;
class TermTranslator;
class Term;

class TranslatingTermConsumer : public TermConsumer {
 public:
  TranslatingTermConsumer(BigTermConsumer* consumer,
						  TermTranslator* translator);
  virtual ~TranslatingTermConsumer();

  virtual void consume(const Term& term);

 private:
  BigTermConsumer* _consumer;
  TermTranslator* _translator;
};

#include "CoefTermConsumer.h"

class CoefBigTermConsumer;

// TODO: move or remove
class TranslatingCoefTermConsumer : public CoefTermConsumer {
 public:
  TranslatingCoefTermConsumer(CoefBigTermConsumer* consumer,
							  TermTranslator* translator);
  virtual ~TranslatingCoefTermConsumer();

  virtual void consume(const mpz_class& coef, const Term& term);

 private:
  CoefBigTermConsumer* _consumer;
  TermTranslator* _translator;
};

#endif
