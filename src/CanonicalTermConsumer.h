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
#ifndef CANONICAL_TERM_CONSUMER_GUARD
#define CANONICAL_TERM_CONSUMER_GUARD

#include "TermConsumer.h"
#include "Ideal.h"

class Term;
class TermTranslator;

#include "Ideal.h"
#include "TermConsumer.h"

// Passes consumed items on in a canonical order. This requires
// storing all items before any can be passed on.
class CanonicalTermConsumer : public TermConsumer {
 public:
  // The translator, if non-null, is used to identify exponents that
  // map to zero, which influences the sorted order.
  CanonicalTermConsumer(auto_ptr<TermConsumer> consumer,
						size_t varCount,
						TermTranslator* translator = 0);

  virtual void beginConsuming();
  virtual void consume(const Term& term);
  virtual void doneConsuming();

 private:
  auto_ptr<TermConsumer> _consumer;
  Ideal _ideal;
  TermTranslator* _translator;
};

#endif
