/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)

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
#include "SquareFreeIdeal.h"

#include "RawSquareFreeIdeal.h"
#include "Ideal.h"
#include "BigIdeal.h"

SquareFreeIdeal::SquareFreeIdeal(): _ideal(0) {}

SquareFreeIdeal::SquareFreeIdeal(const BigIdeal& ideal):
  _names(ideal.getNames()) {
  const size_t varCount = ideal.getVarCount();
  const size_t capacity = ideal.getGeneratorCount();
  _ideal = newRawSquareFreeIdeal(varCount, capacity);
  _ideal->insert(ideal);
}

SquareFreeIdeal::SquareFreeIdeal
(const VarNames& names, RawSquareFreeIdeal* ideal):
  _names(names), _ideal(ideal) {
  ASSERT(ideal != 0);
}

SquareFreeIdeal::~SquareFreeIdeal() {
  clear();
}

SquareFreeIdeal& SquareFreeIdeal::operator=(const BigIdeal& ideal) {
  SquareFreeIdeal copy(ideal);
  swap(copy);
  return *this;
}

void SquareFreeIdeal::minimize() {
  ASSERT(_ideal != 0);
  _ideal->minimize();
}


void SquareFreeIdeal::clear() {
  _names.clear();
  deleteRawSquareFreeIdeal(_ideal);
  _ideal = 0;
}

void SquareFreeIdeal::swap(SquareFreeIdeal& ideal) {
  std::swap(_names, ideal._names);
  std::swap(_ideal, ideal._ideal);
}
