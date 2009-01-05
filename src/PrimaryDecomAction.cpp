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
#include "error.h"
#include "Scanner.h"
#include "SliceFacade.h"
#include "DataType.h"

PrimaryDecomAction::PrimaryDecomAction():
  Action
(staticGetName(),
 "Compute the primary decomposition of monomial ideals.",
 "This feature is experimental which means that it isn't correctly\n"
 "implemented yet. I.e. it doesn't work yet - don't use it other than\n"
 "because you are developing Frobby or curious to see how this\n"
 "functionality is coming along.",
 false),

  _io(DataType::getMonomialIdealType(), DataType::getMonomialIdealListType()) {
}

void PrimaryDecomAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  _sliceParams.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

void PrimaryDecomAction::perform() {
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
  facade.computePrimaryDecomposition();
}

const char* PrimaryDecomAction::staticGetName() {
  return "primdecom";
}
