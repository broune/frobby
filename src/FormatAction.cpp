#include "stdinc.h"
#include "FormatAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"

FormatAction::FormatAction():
  _inputFormat
("iformat",
 "The input format. The available formats are monos, m2 and newmonos.",
 "monos"),
  
  _outputFormat
  ("oformat",
   "The output format. The additional format \"auto\" means use input format.",
   "auto"),
  
  _canonicalize
  ("canon",
   "Sort generators and variables to get canonical representation.",
   false),

  _minimize 
  ("minimize",
  "Remove non-minimial generators. This is currently implemented in a slow way.",
   false) {
}

const char* FormatAction::getName() const {
  return "format";
}

const char* FormatAction::getShortDescription() const {
  return "Change the representation of the input ideal.";
}

const char* FormatAction::getDescription() const {
  return
    "By default, format simply writes the input ideal to output. A number\n"
    "of parameters allows to change the representation of the ideal\n"
    "in various ways before writing it out.";
}

Action* FormatAction::createNew() const {
  return new FormatAction();
}

void FormatAction::obtainParameters(vector<Parameter*>& parameters) {
  parameters.push_back(&_inputFormat);
  parameters.push_back(&_outputFormat);
  parameters.push_back(&_canonicalize);
  parameters.push_back(&_minimize);
  Action::obtainParameters(parameters);
}

void FormatAction::perform() {
  // TODO improve this casting business
  const char* iformat = ((const string&)_inputFormat).c_str();
  const char* oformat = ((const string&)_outputFormat).c_str();

  if (strcmp(oformat, "auto") == 0)
    oformat = iformat;

  IOFacade facade(_printActions);

  if (!facade.isValidMonomialIdealFormat(iformat)) {
    cerr << "ERROR: Unknown input format \"" << iformat<< "\"." << endl;
    exit(0);
  }

  if (!facade.isValidMonomialIdealFormat(oformat)) {
    cerr << "ERROR: Unknown output format \"" << oformat<< "\"." << endl;
    exit(0);
  }

  BigIdeal ideal;
  facade.readIdeal(cin, ideal, iformat);

  if (_minimize)
    ideal.minimize(); // TODO: move this to some facade

  if (_canonicalize)
    ideal.sortUnique(); // TODO: move this to some facade

  facade.writeIdeal(cout, ideal, oformat);
}
