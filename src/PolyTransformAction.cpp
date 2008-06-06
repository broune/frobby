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
#include "PolyTransformAction.h"

#include "Scanner.h"
#include "IOFacade.h"
#include "BigPolynomial.h"

PolyTransformAction::PolyTransformAction():
  Action
(staticGetName(),
 "Transform polynomials. TODO",
 "Transform polynomials. TODO",
 false),
  
  _io(IOHandler::Polynomial, IOHandler::Polynomial),

  _canonicalize
  ("canon",
   "make input canonical. TODO",
   false),

  _sortTerms
  ("sort",
   "Sort terms. TODO",
   false) {
}

void PolyTransformAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _io.obtainParameters(parameters);
  parameters.push_back(&_canonicalize);
  parameters.push_back(&_sortTerms);
}

void PolyTransformAction::perform() {
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  IOFacade facade(_printActions);

  BigPolynomial polynomial;
  facade.readPolynomial(in, polynomial);

  // TODO: use facade
  if (_canonicalize)
	polynomial.sortVariables();
  if (_sortTerms || _canonicalize)
	polynomial.sortTerms();

  facade.writePolynomial(polynomial, _io.getOutputHandler(), stdout);
}

const char* PolyTransformAction::staticGetName() {
  return "ptransform";
}
