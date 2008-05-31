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
#include "TransformAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"
#include "Scanner.h"

TransformAction::TransformAction():
  _canonicalize
  ("canon",
   "Sort generators and variables to get canonical representation.",
   false),

  _sort
  ("sort",
   "Sort generators according to the reverse lexicographic order.",
   false),

  _unique
  ("unique",
   "Remove duplicate generators.",
   false),

  _minimize 
  ("minimize",
  "Remove non-minimial generators.",
   false),

  _deform
  ("deform",
   "Apply a generic deformation to the input ideal.",
   false),
  
  _radical
  ("radical",
   "Take the radical of the input ideal.",
   false) {
}

const char* TransformAction::getName() const {
  return "transform";
}

const char* TransformAction::getShortDescription() const {
  return "Change the representation of the input ideal.";
}

const char* TransformAction::getDescription() const {
  return
    "By default, transform simply writes the input ideal to output. A number\n"
    "of parameters allows to transform the input ideal in varous ways.";
}

Action* TransformAction::createNew() const {
  return new TransformAction();
}

void TransformAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  parameters.push_back(&_canonicalize);
  parameters.push_back(&_minimize);
  parameters.push_back(&_sort);
  parameters.push_back(&_unique);
  parameters.push_back(&_deform);
  parameters.push_back(&_radical);
  Action::obtainParameters(parameters);
}

void TransformAction::perform() {
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  IOFacade facade(_printActions);

  BigIdeal ideal;
  facade.readIdeal(in, ideal);

  IdealFacade idealFacade(_printActions);

  if (_radical)
	idealFacade.takeRadical(ideal);

  if (_minimize || _radical)
	idealFacade.sortAllAndMinimize(ideal);

  if (_deform)
	idealFacade.deform(ideal);

  if (_canonicalize)
	idealFacade.sortVariables(ideal);
  if (_unique )
	idealFacade.sortGeneratorsUnique(ideal);
  else if (_sort || _canonicalize)
	idealFacade.sortGenerators(ideal);

  facade.writeIdeal(ideal, _io.getOutputHandler(), stdout);
}
