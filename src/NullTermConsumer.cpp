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
#include "NullTermConsumer.h"

void NullTermConsumer::beginConsumingList() {
}

void NullTermConsumer::consumeRing(const VarNames& names) {
}

void NullTermConsumer::beginConsuming() {
}

void NullTermConsumer::consume(const Term& term) {
}

void NullTermConsumer::consume(const vector<mpz_class>& term) {
}

void NullTermConsumer::consume
(const Term& term, const TermTranslator& translator) {
}

void NullTermConsumer::doneConsuming() {
}

void NullTermConsumer::doneConsumingList() {
}

void NullTermConsumer::consume(const BigIdeal& ideal) {
}
