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
#include "PrimaryDecomAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"

PrimaryDecomAction::PrimaryDecomAction():
  Action
(staticGetName(),
 "Computes the primary decomposition of monomial ideals.",
 "TODO",
 false) {
}

void PrimaryDecomAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
}

void PrimaryDecomAction::perform() {
  // TODO
}

const char* PrimaryDecomAction::staticGetName() {
  return "primdecom";
}
