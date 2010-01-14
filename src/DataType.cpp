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
#include "DataType.h"

DataType::DataType(const char* name):
  _name(name) {
}

const char* DataType::getName() const {
  return _name;
}

bool DataType::isNull() const {
  return *this == getNullType();
}

bool DataType::operator==(const DataType& type) const {
  return this == &type; // There is only one object of each kind.
}

bool DataType::operator!=(const DataType& type) const {
  return this != &type; // There is only one object of each kind.
}

const DataType& DataType::getNullType() {
  static DataType type("nothing");
  return type;
}

const DataType& DataType::getMonomialIdealType() {
  static DataType type("a monomial ideal");
  return type;
}

const DataType& DataType::getPolynomialType() {
  static DataType type("a polynomial");
  return type;
}
const DataType& DataType::getMonomialIdealListType() {
  static DataType type("a list of monomial ideals");
  return type;
}

const DataType& DataType::getSatBinomIdealType() {
  static DataType type("a saturated binomial ideal");
  return type;
}

vector<const DataType*> DataType::getTypes() {
  vector<const DataType*> types;
  types.push_back(&getMonomialIdealType());
  types.push_back(&getMonomialIdealListType());
  types.push_back(&getPolynomialType());
  types.push_back(&getSatBinomIdealType());
  return types;
}
