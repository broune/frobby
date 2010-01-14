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
#include "IdealFacade.h"
#include "Scanner.h"
#include "BigIdeal.h"
#include "BigTermConsumer.h"
#include "SliceFacade.h"
#include "SliceParams.h"
#include "NullTermConsumer.h"
#include "SliceParameters.h"
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

  _squareFreeAndMinimal
  ("squareFreeAndMinimal",
   "State that the input ideal is square free and minimally generated. This\n"
   "can speed up the the computation, but will result in unpredictable\n"
   "behavior if it is not true.",
   false),

  _useSlice
  ("useSlice",
   "Use the Slice Algorithm to compute the dimension instead of the usual\n"
   "algorithm.",
   false),

  _io(DataType::getMonomialIdealType(), DataType::getNullType()) {
}

void DimensionAction::obtainParameters(vector<Parameter*>& parameters) {
  parameters.push_back(&_codimension);
  parameters.push_back(&_squareFreeAndMinimal);
  parameters.push_back(&_useSlice);
  _io.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

void DimensionAction::perform() {
  mpz_class result;
  if (_useSlice) {
    SliceParams params;
    params.useIndependenceSplits(false); // not supported
    validateSplit(params, true, false);
    SliceFacade facade(params, DataType::getNullType());
    result = facade.computeDimension(_codimension);
  } else {
    BigIdeal ideal;
    Scanner in(_io.getInputFormat(), stdin);
    _io.autoDetectInputFormat(in);
    _io.validateFormats();

    IOFacade ioFacade(_printActions);
    ioFacade.readIdeal(in, ideal);
    in.expectEOF();

    IdealFacade facade(_printActions);
    result = facade.computeDimension(ideal,
                                     _codimension,
                                     _squareFreeAndMinimal);
  }
  gmp_fprintf(stdout, "%Zd\n", result.get_mpz_t());
}

const char* DimensionAction::staticGetName() {
  return "dimension";
}
