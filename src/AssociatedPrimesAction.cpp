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
#include "AssociatedPrimesAction.h"

#include "SliceFacade.h"
#include "SliceParams.h"
#include "DataType.h"

AssociatedPrimesAction::AssociatedPrimesAction():
  Action
(staticGetName(),
 "Compute the associated primes of the input ideal.",
 "Computes the associated prime ideals of the input monomial ideal. The\n"
 "computation is accomplished using irreducible decomposition. The quality "
 "of the\n"
 "algorithm for computing associated primes is expected to be much improved "
 "in a\n"
 "future version of Frobby.",
 false),

  _io(DataType::getMonomialIdealType(), DataType::getMonomialIdealType()) {
}

void AssociatedPrimesAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  _sliceParams.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

void AssociatedPrimesAction::perform() {
  SliceParams params(_params);
  validateSplit(params, true, false);
  SliceFacade facade(params, DataType::getMonomialIdealListType());
  facade.computeAssociatedPrimes();
}

const char* AssociatedPrimesAction::staticGetName() {
  return "assoprimes";
}
