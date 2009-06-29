/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

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
#include "DimensionAction.h"

#include "DataType.h"
#include "IOFacade.h"
#include "SliceFacade.h"
#include "Scanner.h"
#include "BigIdeal.h"
#include "BigTermConsumer.h"
#include "NullTermConsumer.h"
#include "error.h"

#include <algorithm>

/** @todo Expand the description of this action. */
DimensionAction::DimensionAction():
  Action
(staticGetName(),
 "Compute the (co)dimension of the input ideal.",
 "Compute the dimension or codimension of the input ideal. This is the\n"
 "dimension of the zero set of the ideal. The ideal containing the identity\n"
 "has an empty zero set, and we define the dimension of this to be -1.\n"
 "All other ideals have non-negative dimension.",
 false),

  _codimension
  ("codim",
   "Compute the codimension instead of the dimension. The codimension is the\n"
   "number of variables in the polynomial ring minus the dimension.",
   false),

  _sliceParams(true, false),
  _io(DataType::getMonomialIdealType(), DataType::getNullType()) {
  _sliceParams.setSplit("degree");
}

void DimensionAction::obtainParameters(vector<Parameter*>& parameters) {
  parameters.push_back(&_codimension);
  _io.obtainParameters(parameters);
  _sliceParams.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

void DimensionAction::perform() {
  _sliceParams.validateSplit(true, true);

  BigIdeal ideal;
  {
	Scanner in(_io.getInputFormat(), stdin);
	_io.autoDetectInputFormat(in);
	_io.validateFormats();

	IOFacade ioFacade(_printActions);
	ioFacade.readIdeal(in, ideal);
	in.expectEOF();
  }
  size_t varCount = ideal.getVarCount();

  NullTermConsumer dummy;
  SliceFacade facade(ideal, &dummy, _printActions);
  _sliceParams.apply(facade);

  mpz_class dimension;
  facade.computeDimension(dimension);
  
  if (_codimension) {
	mpz_class codimension = varCount - dimension;
	gmp_fprintf(stdout, "%Zd\n", codimension.get_mpz_t());
  } else
	gmp_fprintf(stdout, "%Zd\n", dimension.get_mpz_t());
}

const char* DimensionAction::staticGetName() {
  return "dimension";
}
