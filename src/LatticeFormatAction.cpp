#include "stdinc.h"
#include "LatticeFormatAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "fplllIO.h"
#include "LatticeFacade.h"
#include "Scanner.h"

LatticeFormatAction::LatticeFormatAction():
  _inputFormat
("iformat",
 "The input format. The available formats are 4ti2 and fplll.",
 "4ti2"),
  
  _outputFormat
  ("oformat",
   "The output format. The additional format \"input\" means use input format.",
   "input"),

  _zero
  ("zero",
   "Adjust lattice basis to increase the number of zero entries.",
   false) {
}

const char* LatticeFormatAction::getName() const {
  return "latformat";
}

const char* LatticeFormatAction::getShortDescription() const {
  return "Change the representation of the input lattice.";
}

const char* LatticeFormatAction::getDescription() const {
  return
    "By default, latformat simply writes the input lattice to output.\n"
    "The main functionality is to change the format.";
}

Action* LatticeFormatAction::createNew() const {
  return new LatticeFormatAction();
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

  if (oformat == "input")
    oformat = iformat;

  IOFacade facade(_printActions);

  if (!facade.isValidLatticeFormat(iformat)) {
    fprintf(stderr, "ERROR: Unknown input format \"%s\".\n", iformat.c_str());
    exit(1);
  }

  if (!facade.isValidLatticeFormat(oformat)) {
    fprintf(stderr, "ERROR: Unknown output format \"%s\".\n", oformat.c_str());
    exit(1);
  }

  BigIdeal basis;
  Scanner in(iformat, stdin);
  facade.readLattice(in, basis);

  if (_zero) {
    LatticeFacade latticeFacade(_printActions);
    latticeFacade.makeZeroesInLatticeBasis(basis);
  }

  facade.writeLattice(stdout, basis, oformat);
}
