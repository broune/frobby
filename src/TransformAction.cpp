#include "stdinc.h"
#include "TransformAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"

TransformAction::TransformAction():
  _inputFormat
("iformat",
 "The input format. The available formats are monos, m2, 4ti2, null and newmonos.",
 "monos"),
  
  _outputFormat
  ("oformat",
   "The output format. The additional format \"auto\" means use input format.",
   "auto"),
  
  _canonicalize
  ("canon",
   "Sort generators and variables to get canonical representation.",
   false),

  _sort
  ("sort",
   "Sort generators according to the reverse lexicographic order.",
   false),

  _unique
  ("unique",
   "Remove duplicate generators.",
   false),

  _minimize 
  ("minimize",
  "Remove non-minimial generators.",
   false) {
}

const char* TransformAction::getName() const {
  return "transform";
}

const char* TransformAction::getShortDescription() const {
  return "Change the representation of the input ideal.";
}

const char* TransformAction::getDescription() const {
  return
    "By default, transform simply writes the input ideal to output. A number\n"
    "of parameters allows to transform the input ideal in varous ways.";
}

Action* TransformAction::createNew() const {
  return new TransformAction();
}

void TransformAction::obtainParameters(vector<Parameter*>& parameters) {
  parameters.push_back(&_inputFormat);
  parameters.push_back(&_outputFormat);
  parameters.push_back(&_canonicalize);
  parameters.push_back(&_minimize);
  parameters.push_back(&_sort);
  parameters.push_back(&_unique);
  Action::obtainParameters(parameters);
}

void TransformAction::perform() {
  string iformat = _inputFormat.getValue();
  string oformat = _outputFormat.getValue();

  if (oformat == "auto")
    oformat = iformat;

  IOFacade facade(_printActions);

  if (!facade.isValidMonomialIdealFormat(iformat.c_str())) {
    fprintf(stderr, "ERROR: Unknown input format \"%s\".\n", iformat.c_str());
    exit(1);
  }

  if (!facade.isValidMonomialIdealFormat(oformat.c_str())) {
    fprintf(stderr, "ERROR: Unknown output format \"%s\".\n", oformat.c_str());
    exit(1);
  }

  BigIdeal ideal;
  facade.readIdeal(stdin, ideal, iformat.c_str());

  IdealFacade idealFacade(_printActions);

  if (_minimize)
    idealFacade.sortAllAndMinimize(ideal, stdout, oformat.c_str());
  else {
	if (_canonicalize)
	  idealFacade.sortVariables(ideal);

	if (_unique)
	  idealFacade.sortGeneratorsUnique(ideal);
	else if (_sort || _canonicalize)
	  idealFacade.sortGenerators(ideal);

	facade.writeIdeal(stdout, ideal, oformat.c_str());
  }
}
