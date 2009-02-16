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
#ifndef NULL_TERM_CONSUMER_GUARD
#define NULL_TERM_CONSUMER_GUARD

#include "BigTermConsumer.h"

// This class is an instance of the Null Object pattern. None of its
// methods do anything.
class NullTermConsumer : public BigTermConsumer {
 public:
  virtual void beginConsumingList();
  virtual void consumeRing(const VarNames& names);
  virtual void beginConsuming();

  virtual void consume(const Term& term);
  virtual void consume(const vector<mpz_class>& term);
  virtual void consume(const Term& term, const TermTranslator& translator);

  virtual void doneConsuming();
  virtual void doneConsumingList();

  virtual void consume(const BigIdeal& ideal);
};

#endif
