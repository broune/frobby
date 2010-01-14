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
#include "error.h"

GenerateFrobeniusAction::GenerateFrobeniusAction():
  Action
(staticGetName(),
 "Generate a random Frobenius problem instance.",
 "Generate a random Frobenius problem instance.",
 false),

  _entryCount
  ("entryCount",
   "The number of entries in the random instance.",
   4),

  _maxEntryDigits
  ("maxEntryDigits",
   "The largest allowed number of decimal digits for entries in the\n"
   "random instance.",
   2) {
}

void GenerateFrobeniusAction::
obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  parameters.push_back(&_entryCount);
  parameters.push_back(&_maxEntryDigits);
}

void GenerateFrobeniusAction::perform() {
  vector<mpz_class> instance;

  if (_entryCount < 1)
    reportError("There must be at least one entry.");
  if (_maxEntryDigits < 1)
    reportError("The largest allowed number of digits must be at least 1.");

  mpz_class maxEntry;
  mpz_ui_pow_ui(maxEntry.get_mpz_t(), 10, _maxEntryDigits.getValue());

  GenerateDataFacade generator(_printActions);
  generator.generateFrobeniusInstance
    (instance, _entryCount.getValue(), maxEntry);

  IOFacade ioFacade(_printActions);
  ioFacade.writeFrobeniusInstance(stdout, instance);
}

const char* GenerateFrobeniusAction::staticGetName() {
  return "genfrob";
}
