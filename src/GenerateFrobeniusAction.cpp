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
#include "GenerateFrobeniusAction.h"

#include "BigIdeal.h"
#include "GenerateDataFacade.h"
#include "IOFacade.h"

GenerateFrobeniusAction::GenerateFrobeniusAction():
  Action
(staticGetName(),
 "Generate a random Frobenius problem instance.",
 "Generate a random Frobenius problem instance.",
 false) {
}

void GenerateFrobeniusAction::
obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
}

void GenerateFrobeniusAction::perform() {
  vector<mpz_class> instance;

  GenerateDataFacade generator(_printActions);
  generator.generateFrobeniusInstance(instance);

  IOFacade ioFacade(_printActions);
  ioFacade.writeFrobeniusInstance(stdout, instance);
}

const char* GenerateFrobeniusAction::staticGetName() {
  return "genfrob";
}
