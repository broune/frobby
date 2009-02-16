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
#include "PolynomialFacade.h"

#include "BigPolynomial.h"

PolynomialFacade::PolynomialFacade(bool printActions):
  Facade(printActions) {
}

void PolynomialFacade::sortTerms(BigPolynomial& poly) {
  beginAction("Sorting terms of polynomial.\n");

  poly.sortTermsReverseLex();

  endAction();
}

void PolynomialFacade::sortVariables(BigPolynomial& poly) {
  beginAction("Sorting variables of polynomial.\n");

  poly.sortVariables();

  endAction();
}
