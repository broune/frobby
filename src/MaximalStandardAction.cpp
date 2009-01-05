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
#include "MaximalStandardAction.h"

#include "BigIdeal.h"
#include "SliceFacade.h"
#include "IOFacade.h"
#include "Scanner.h"
#include "DataType.h"

MaximalStandardAction::MaximalStandardAction():
  Action
(staticGetName(),
 "Compute the maximal standard monomials of the input ideal.",
 "Compute the maximal standard monomials of the input ideal. A standard "
 "monomial\nis a monomial that does not lie in the ideal, while a "
 "maximal\nstandard monomial m is a standard monomial such that m * v does "
 "lie in the\nideal for every variable v in the ambient polynomial ring of I.",
 false),

  _io(DataType::getMonomialIdealType(), DataType::getMonomialIdealType()) {
}

const char* MaximalStandardAction::staticGetName() {
  return "maxstandard";
}

void MaximalStandardAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  _sliceParams.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

void MaximalStandardAction::perform() {
  BigIdeal ideal;

  _sliceParams.validateSplit(true, false);

  {
	Scanner in(_io.getInputFormat(), stdin);
	_io.autoDetectInputFormat(in);
	_io.validateFormats();

	IOFacade ioFacade(_printActions);
	ioFacade.readIdeal(in, ideal);
	in.expectEOF();
  }

  auto_ptr<IOHandler> output = _io.createOutputHandler();
  SliceFacade facade(ideal, output.get(), stdout, _printActions);
  _sliceParams.apply(facade);
  facade.computeMaximalStandardMonomials();
}
