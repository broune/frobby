/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "BigattiState.h"

#include "BigattiHilbertAlgorithm.h"

BigattiState::BigattiState(BigattiHilbertAlgorithm* algorithm,
						   const Ideal& ideal, const Term& multiply):
  _algorithm(algorithm),
  _ideal(ideal),
  _multiply(multiply) {
  ASSERT(_algorithm != 0);
  ASSERT(_ideal.getVarCount() == _multiply.getVarCount());
}

const Ideal& BigattiState::getIdeal() const {
  return _ideal;
}

const Term& BigattiState::getMultiply() const {
  return _multiply;
}

Ideal& BigattiState::getIdeal() {
  return _ideal;
}

Term& BigattiState::getMultiply() {
  return _multiply;
}

void BigattiState::colonStep(const Term& pivot) {
  ASSERT(pivot.getVarCount() == _ideal.getVarCount());

  _ideal.colonReminimize(pivot);
  _multiply.product(_multiply, pivot);
}

void BigattiState::addStep(const Term& pivot) {
  _ideal.insertReminimize(pivot);
}

void BigattiState::run(TaskEngine& tasks) {
  _algorithm->processState(auto_ptr<BigattiState>(this));
}

void BigattiState::dispose() {
  _algorithm->freeState(auto_ptr<BigattiState>(this));
}
