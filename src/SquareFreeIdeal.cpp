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

SquareFreeIdeal::SquareFreeIdeal(): _ideal(0), _capacity(0) {}

SquareFreeIdeal::SquareFreeIdeal
(const SquareFreeIdeal& ideal, size_t capacity):
  _names(ideal.getNames()) {
  _capacity = capacity;
  if (_capacity < ideal.getGeneratorCount())
	_capacity = ideal.getGeneratorCount();
  _ideal = newRawSquareFreeIdeal(_names.getVarCount(), _capacity);
  _ideal->insert(*ideal._ideal);
}

SquareFreeIdeal::SquareFreeIdeal(const BigIdeal& ideal):
  _names(ideal.getNames()) {
  _capacity = ideal.getGeneratorCount();
  _ideal = newRawSquareFreeIdeal(_names.getVarCount(), _capacity);
  _ideal->insert(ideal);
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

void SquareFreeIdeal::renameVars(const VarNames& names) {
  ASSERT(_ideal != 0);
  ASSERT(_names.getVarCount() == names.getVarCount());
  _names = names;
}

void SquareFreeIdeal::insertIdentity() {
  if (getGeneratorCount() == getCapacity())
	reserve(getCapacity() == 0 ? 16 : getCapacity() * 2);
  _ideal->insertIdentity();
}

void SquareFreeIdeal::insert(Word* term) {
  ASSERT(term != 0);
  ASSERT(getGeneratorCount() <= getCapacity());
  if (getGeneratorCount() == getCapacity())
	reserve(getCapacity() == 0 ? 16 : getCapacity() * 2);
  ASSERT(getGeneratorCount() < getCapacity());
  _ideal->insert(term);
}

bool SquareFreeIdeal::insert(const std::vector<std::string>& term) {
  ASSERT(term.size() == getVarCount());
  ASSERT(getGeneratorCount() <= getCapacity());
  if (getGeneratorCount() == getCapacity())
	reserve(getCapacity() == 0 ? 16 : getCapacity() * 2);
  ASSERT(getGeneratorCount() < getCapacity());
  return _ideal->insert(term);
}

void SquareFreeIdeal::clear() {
  _names.clear();
  deleteRawSquareFreeIdeal(_ideal);
  _ideal = 0;
  _capacity = 0;
}

void SquareFreeIdeal::swap(SquareFreeIdeal& ideal) {
  std::swap(_names, ideal._names);
  std::swap(_ideal, ideal._ideal);
  std::swap(_capacity, ideal._capacity);
}

void SquareFreeIdeal::reserve(size_t capacity) {
  if (getCapacity() < capacity)
	SquareFreeIdeal(*this, capacity).swap(*this);
}
