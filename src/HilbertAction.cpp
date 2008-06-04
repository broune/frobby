/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABIL-ITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "HilbertAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "SliceFacade.h"
#include "Scanner.h"

HilbertAction::HilbertAction():
  _io(IOHandler::MonomialIdeal, IOHandler::Polynomial),

  _multigraded
  ("multigraded",
   "Use the trivial multi-grading. Otherwise substitute t for each variable.",
   true),

  _canonical
  ("canon",
   "Collect and sort terms to get a canonical representation.",
   false) {
}

const char* HilbertAction::getName() const {
  return "hilbert";
}

const char* HilbertAction::getShortDescription() const {
  return "Compute the Hilbert-Poincare series of the input ideal.";
}

const char* HilbertAction::getDescription() const {
  return getShortDescription();
}

Action* HilbertAction::createNew() const {
  return new HilbertAction();
}

void HilbertAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  parameters.push_back(&_multigraded);
  parameters.push_back(&_canonical);
  Action::obtainParameters(parameters);
}

void HilbertAction::perform() {
  BigIdeal ideal;

  {
	Scanner in(_io.getInputFormat(), stdin);
	_io.autoDetectInputFormat(in);
	_io.validateFormats();

	IOFacade facade(_printActions);
	facade.readIdeal(in, ideal);
  }

  SliceFacade facade(ideal, _io.getOutputHandler(), stdout, _printActions);
  if (_multigraded)
	facade.computeMultigradedHilbertSeries(_canonical);
  else
	facade.computeUnivariateHilbertSeries();
}
