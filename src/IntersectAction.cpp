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
#include "IntersectAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IntersectFacade.h"
#include "Scanner.h"

IntersectAction::IntersectAction():
  _io(IOHandler::MonomialIdealList, IOHandler::MonomialIdeal) {
}

const char* IntersectAction::getName() const {
  return "intersect";
}

const char* IntersectAction::getShortDescription() const {
  return "Intersect the input ideals.";
}

const char* IntersectAction::getDescription() const {
  return
"Computes the intersection of the input ideals. Simply concatenate the textual\n"
"representations of the ideals in order to intersect them.\n"
    "\n"
    "Note that this operation is currently implemented in a rather slow way.";
}

Action* IntersectAction::createNew() const {
  return new IntersectAction();
}

void IntersectAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _io.obtainParameters(parameters);
}

void IntersectAction::perform() {
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  vector<BigIdeal*> ideals;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeals(in, ideals);

  IntersectFacade facade(_printActions);
  BigIdeal* intersection = facade.intersect(ideals);

  ioFacade.writeIdeal(*intersection, _io.getOutputHandler(), stdout );

  delete intersection;
  for (size_t i = 0; i < ideals.size(); ++i)
    delete ideals[i];
}
