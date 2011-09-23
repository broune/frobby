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
#include "LatticeFormatAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "fplllIO.h"
#include "LatticeFacade.h"
#include "Scanner.h"
#include "error.h"
#include "IOHandler.h"

LatticeFormatAction::LatticeFormatAction():
Action
(staticGetName(),
 "Change the representation of the input lattice.",
 "By default, latformat simply writes the input lattice to output.\n"
 "The main functionality is to change the format.",
 false),

_inputFormat
  ("iformat",
   "The input format. The available formats are 4ti2 and fplll.",
   "4ti2"),

  _outputFormat
  ("oformat",
   "The output format. The additional format \"" +
   getFormatNameIndicatingToUseInputFormatAsOutputFormat() +
   "\" means use input format.",
   getFormatNameIndicatingToUseInputFormatAsOutputFormat()),

  _zero
  ("zero",
   "Adjust lattice basis to increase the number of zero entries.",
   false) {
}

void LatticeFormatAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  parameters.push_back(&_inputFormat);
  parameters.push_back(&_outputFormat);
  parameters.push_back(&_zero);
}

void LatticeFormatAction::perform() {
  string iformat = _inputFormat.getValue();
  string oformat = _outputFormat.getValue();

  if (oformat == getFormatNameIndicatingToUseInputFormatAsOutputFormat())
    oformat = iformat;

  IOFacade facade(_printActions);

  if (!facade.isValidLatticeFormat(iformat))
    reportError("Unknown lattice input format \"" + iformat + "\".");
  if (!facade.isValidLatticeFormat(oformat))
    reportError("Unknown lattice output format \"" + oformat + "\".");

  BigIdeal basis;
  Scanner in(iformat, stdin);
  facade.readLattice(in, basis);
  in.expectEOF();

  if (_zero) {
    LatticeFacade latticeFacade(_printActions);
    latticeFacade.makeZeroesInLatticeBasis(basis);
  }

  facade.writeLattice(stdout, basis, oformat);
}

const char* LatticeFormatAction::staticGetName() {
  return "latformat";
}
