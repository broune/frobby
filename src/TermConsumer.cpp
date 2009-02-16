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
#include "TermConsumer.h"

#include "Term.h"
#include "Ideal.h"

TermConsumer::~TermConsumer() {
}

void TermConsumer::consumeRing(const VarNames& names) {
}

void TermConsumer::beginConsumingList() {
}

void TermConsumer::doneConsumingList() {
}

void TermConsumer::consume(const Ideal& ideal) {
  beginConsuming();
  Term tmp(ideal.getVarCount());
  for (size_t term = 0; term < ideal.getGeneratorCount(); ++term) {
	tmp = ideal[term];
	consume(tmp);
  }
  doneConsuming(); 
}
