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
#include "FrobeniusAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IrreducibleDecomFacade.h"
#include "Scanner.h"

FrobeniusAction::FrobeniusAction():
  _decomParameters(true),
  _displaySolution
("vector",
 "Display the vector that achieves the optimal value.",
 false) {
  _decomParameters.setSplit("frob");
}

const char* FrobeniusAction::getName() const {
  return "frobgrob";
}

const char* FrobeniusAction::getShortDescription() const {
  return "Compute Frobenius number using a Grobner basis algorithm.";
}

const char* FrobeniusAction::getDescription() const {
  return
"Compute the Frobenius number of the passed-in Frobenius instance. This instance\n"
"must be preceded in the input by a deg-rev-lex lattice ideal Grobner basis as\n"
"produced by the program 4ti2.\n"
"\n"
"The algorithm for this uses irreducible decomposition to compute the Frobenius\n"
"number, which is why this action accepts parameters related to that. See the\n"
"paper \"Solving Thousand Digit Frobenius Problems Using Grobner Bases\"\n"
"at www.broune.com for more details.";
}

Action* FrobeniusAction::createNew() const {
  return new FrobeniusAction();
}

void FrobeniusAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _decomParameters.obtainParameters(parameters);

  parameters.push_back(&_displaySolution);
}

void FrobeniusAction::perform() {
  vector<mpz_class> instance;
  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  Scanner in("", stdin);
  ioFacade.readFrobeniusInstanceWithGrobnerBasis(in, ideal, instance);

  IrreducibleDecomFacade facade(_printActions, _decomParameters);

  mpz_class frobeniusNumber;
  vector<mpz_class> vector;
  facade.computeFrobeniusNumber(instance, ideal, frobeniusNumber, vector);

  if (_displaySolution) {
	fputc('(', stdout);
	for (size_t i = 0; i < vector.size(); ++i)
	  gmp_fprintf(stdout, "%s%Zd", i == 0 ? "" : ", ", vector[i].get_mpz_t());
	fputs(")\n", stdout);
  }

  gmp_fprintf(stdout, "%Zd\n", frobeniusNumber.get_mpz_t());
}
