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
#include "DataType.h"

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

  _univariate
  ("univariate",
   "Output a univariate polynomial by substituting t for each variable.",
   false) {
}

void HilbertAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  parameters.push_back(&_univariate);
  _sliceParams.obtainParameters(parameters);
  Action::obtainParameters(parameters);
}

void HilbertAction::perform() {
  BigIdeal ideal;

  _sliceParams.validateSplit(false, false);

  {
	Scanner in(_io.getInputFormat(), stdin);
	_io.autoDetectInputFormat(in);
	_io.validateFormats();

	IOFacade facade(_printActions);
	facade.readIdeal(in, ideal);
	in.expectEOF();
  }

  auto_ptr<IOHandler> output = _io.createOutputHandler();
  SliceFacade facade(ideal, output.get(), stdout, _printActions);
  _sliceParams.apply(facade);
  if (_univariate)
	facade.computeUnivariateHilbertSeries();
  else
	facade.computeMultigradedHilbertSeries();
}

const char* HilbertAction::staticGetName() {
  return "hilbert";
}
