/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 Bjarke Hammersholt Roune (www.broune.com)

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
#include "EulerAction.h"

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
EulerAction::EulerAction():
  Action
(staticGetName(),
 "Compute the Euler characteristic of a simplicial complex.",
 "Compute the Euler characteristic of a simplicial complex.",
 false),

  _io(DataType::getMonomialIdealType(), DataType::getNullType()) {
}

void EulerAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

void EulerAction::perform() {
  BigIdeal ideal;
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(in, ideal);
  in.expectEOF();

  IdealFacade facade(_printActions);
  mpz_class result = facade.computeEuler(ideal);

  gmp_fprintf(stdout, "%Zd\n", result.get_mpz_t());
}

const char* EulerAction::staticGetName() {
  return "euler";
}
