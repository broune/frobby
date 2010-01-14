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
#include "SliceFacade.h"
#include "SliceParams.h"
#include "BigTermRecorder.h"
#include "Scanner.h"
#include "display.h"

FrobeniusAction::FrobeniusAction():
  Action
(staticGetName(),
 "Compute Frobenius number using a Grobner basis algorithm.",
 "Compute the Frobenius number of the passed-in Frobenius instance. This "
 "instance\n"
 "must be preceded in the input by a deg-rev-lex lattice ideal Grobner basis "
 "as\n"
 "produced by the program 4ti2.\n\n"
 "The algorithm for this uses irreducible decomposition to compute the "
 "Frobenius\n"
 "number, which is why this action accepts parameters related to that. See "
 "the\n"
 "paper \"Solving Thousand Digit Frobenius Problems Using Grobner Bases\"\n"
 "at www.broune.com for more details.",
 false),

  _sliceParams(true, false),
  _displaySolution
("vector",
 "Display the vector that achieves the optimal value.",
 false) {
  _sliceParams.setSplit("frob");
}

void FrobeniusAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _sliceParams.obtainParameters(parameters);

  parameters.push_back(&_displaySolution);
}

void FrobeniusAction::perform() {
  displayNote
    ("The action frobgrob is DEPRECATED, and will be removed in a future "
     "release of Frobby. Use the action optimize with options "
     "-chopFirstAndSubtract and -maxStandard instead to get the same effect.");

  SliceParams params(_params);
  validateSplit(params, true, true);

  vector<mpz_class> instance;
  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  Scanner in("", stdin);
  ioFacade.readFrobeniusInstanceWithGrobnerBasis(in, ideal, instance);
  in.expectEOF();

  vector<mpz_class> shiftedDegrees(instance.begin() + 1, instance.end());
  vector<mpz_class> bigVector;

  BigTermRecorder recorder;

  SliceFacade facade(params, ideal, recorder);
  mpz_class dummy;
  facade.solveStandardProgram(shiftedDegrees, dummy, false);

  BigIdeal maxSolution = *(recorder.releaseIdeal());

  ASSERT(maxSolution.getGeneratorCount() == 1);
  bigVector = maxSolution[0];

  mpz_class frobeniusNumber = -instance[0];
  for (size_t i = 1; i < instance.size(); ++i)
    frobeniusNumber += bigVector[i - 1] * instance[i];

  if (_displaySolution) {
    fputs("(-1", stdout);
    for (size_t i = 0; i < bigVector.size(); ++i)
      gmp_fprintf(stdout, ", %Zd", bigVector[i].get_mpz_t());
    fputs(")\n", stdout);
  }

  gmp_fprintf(stdout, "%Zd\n", frobeniusNumber.get_mpz_t());
}

const char* FrobeniusAction::staticGetName() {
  return "frobgrob";
}

bool FrobeniusAction::displayAction() const {
  return false;
}
