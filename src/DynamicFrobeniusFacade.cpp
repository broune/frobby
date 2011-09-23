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
#include "DynamicFrobeniusFacade.h"
#include "dynamicFrobeniusAlgorithm.h"

DynamicFrobeniusFacade::
DynamicFrobeniusFacade(bool printActions):
  Facade(printActions) {
}

void DynamicFrobeniusFacade::
computeFrobeniusNumber(const vector<mpz_class>& instance,
               mpz_class& frobeniusNumber) {
  beginAction
    ("Computing Frobenius number using dynamic programming algorithm.");

  frobeniusNumber = dynamicFrobeniusAlgorithm(instance);

  endAction();
}
