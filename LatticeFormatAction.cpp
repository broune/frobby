#include "stdinc.h"
#include "LatticeFormatAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "fplllIO.h"

LatticeFormatAction::LatticeFormatAction():
  _inputFormat
("iformat",
 "The input format. The available formats are 4ti2 and fplll.",
 "4ti2"),
  
  _outputFormat
  ("oformat",
   "The output format. The additional format \"auto\" means use input format.",
   "auto"),

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
  // TODO improve this casting business
  const char* iformat = ((const string&)_inputFormat).c_str();
  const char* oformat = ((const string&)_outputFormat).c_str();

  if (strcmp(oformat, "auto") == 0)
    oformat = iformat;

  IOFacade facade(_printActions);

  if (!facade.isValidLatticeFormat(iformat)) {
    cerr << "ERROR: Unknown input format \"" << iformat<< "\"." << endl;
    exit(0);
  }

  if (!facade.isValidLatticeFormat(oformat)) {
    cerr << "ERROR: Unknown output format \"" << oformat<< "\"." << endl;
    exit(0);
  }

  BigIdeal basis;
  facade.readLattice(cin, basis, iformat);

  // TODO: more this to some facade.
  if (_zero)
    fplll::makeZeroesInLatticeBasis(basis);

  facade.writeLattice(cout, basis, oformat);
}
