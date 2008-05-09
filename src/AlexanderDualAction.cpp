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
#include "AlexanderDualAction.h"

#include "BigIdeal.h"
#include "IrreducibleDecomFacade.h"
#include "IOFacade.h"
#include "Scanner.h"
#include "IOHandler.h"

const char* AlexanderDualAction::getName() const {
  return "alexdual";
}

const char* AlexanderDualAction::getShortDescription() const {
  return "Compute the Alexander dual of the input ideal.";
}

const char* AlexanderDualAction::getDescription() const {
  return
"Compute the alexander dual of the input monomial ideal.\n"
"\n"
"The computation is done using irreducible decomposition, which is why there are\n"
"a number of options related to that. See the help topic on irrdecom for details.";
}

Action* AlexanderDualAction::createNew() const {
  return new AlexanderDualAction();
}

void AlexanderDualAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _decomParameters.obtainParameters(parameters);
  _io.obtainParameters(parameters);
}

void AlexanderDualAction::perform() {
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  BigIdeal ideal;
  vector<mpz_class> point;

  IOFacade ioFacade(_printActions);
  bool pointSpecified =
	ioFacade.readAlexanderDualInstance(in, ideal, point);

  BigTermConsumer* consumer =
	IOHandler::getIOHandler
	(_io.getOutputFormat())->createWriter(stdout, ideal.getNames());

  IrreducibleDecomFacade facade(_printActions, _decomParameters);
  if (pointSpecified)
	facade.computeAlexanderDual(ideal, &point, consumer);
  else
	facade.computeAlexanderDual(ideal, 0, consumer);

  delete consumer;
}
