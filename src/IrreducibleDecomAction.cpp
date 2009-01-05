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
#include "IrreducibleDecomAction.h"

#include "BigIdeal.h"
#include "SliceFacade.h"
#include "IOFacade.h"
#include "Scanner.h"
#include "DataType.h"

IrreducibleDecomAction::IrreducibleDecomAction():
  Action
(staticGetName(),
 "Compute the irreducible decomposition of the input ideal.",
 "Compute the irredundant irreducible decomposition of the input monomial "
 "ideal.\n\n"
 "The decomposition is computed using the Slice Algorithm. This algorithm is\n"
 "described in the paper `The Slice Algorithm For Irreducible Decomposition "
 "of\n"
 "Monomial Ideals', which is available at www.broune.com .",
 false),

  _encode("encode",
		  "Encode the decomposition as monomials generating an ideal.",
		  false),

  _io(DataType::getMonomialIdealType(), DataType::getMonomialIdealType()) {
}

void IrreducibleDecomAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  parameters.push_back(&_encode);
  _sliceParams.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

void IrreducibleDecomAction::perform() {
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
  facade.computeIrreducibleDecomposition(_encode);
}

const char* IrreducibleDecomAction::staticGetName() {
  return "irrdecom";
}
