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

#include "SliceFacade.h"
#include "SliceParams.h"
#include "BigattiFacade.h"
#include "BigattiParams.h"
#include "ScarfFacade.h"
#include "DataType.h"
#include "ScarfParams.h"
#include "error.h"

HilbertAction::HilbertAction():
  Action
(staticGetName(),
 "Compute the Hilbert-Poincare series of the input ideal.",
 "Compute the multigraded Hilbert-Poincare series of the input\n"
 "ideal. Use the -univariate option to get the univariate series.\n\n"
 "The Hilbert-Poincare series of a monomial ideal is the sum of all\n"
 "monomials not in the ideal. This sum can be written as a (finite)\n"
 "rational function with (x1-1)(x2-1)...(xn-1) in the denominator,\n"
 "assuming the variables of the ring are x1,x2,...,xn. This action\n"
 "computes the polynomial in the numerator of this fraction.",
 false),

  _io(DataType::getMonomialIdealType(), DataType::getPolynomialType()),

  _sliceParams(false, true, true),

  _univariate
  ("univariate",
   "Output a univariate polynomial by substituting t for each variable.",
   false),

  _algorithm
  ("algorithm",
   "Which algorithm to use. Options are slice, bigatti and deform.",
   "bigatti") {

  _params.add(_io);
  _params.add(_sliceParams);
  _params.add(_univariate);
  _params.add(_algorithm);

  addScarfParams(_params);
}

void HilbertAction::perform() {
  if (_algorithm.getValue() == "bigatti") {
    BigattiParams params(_params);
    BigattiFacade facade(params);
    if (_univariate)
      facade.computeUnivariateHilbertSeries();
    else
      facade.computeMultigradedHilbertSeries();
  } else if (_algorithm.getValue() == "slice") {
    SliceParams params(_params);
    validateSplit(params, false, false);
    SliceFacade sliceFacade(params, DataType::getPolynomialType());
    if (_univariate)
      sliceFacade.computeUnivariateHilbertSeries();
    else
      sliceFacade.computeMultigradedHilbertSeries();
  } else if (_algorithm.getValue() == "deform") {
    ScarfParams params(_params);
    ScarfFacade facade(params);
    if (_univariate)
      facade.computeUnivariateHilbertSeries();
    else
      facade.computeMultigradedHilbertSeries();
  } else
    reportError("Unknown Hilbert-Poincare series algorithm \"" +
                _algorithm.getValue() + "\".");
}

const char* HilbertAction::staticGetName() {
  return "hilbert";
}
