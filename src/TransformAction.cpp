#include "stdinc.h"
#include "TransformAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"

TransformAction::TransformAction():
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
   false),

  _deform
  ("deform",
   "Apply a generic deformation to the input ideal.",
   false),
  
  _radical
  ("radical",
   "Take the radical of the input ideal.",
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
  _io.obtainParameters(parameters);
  parameters.push_back(&_canonicalize);
  parameters.push_back(&_minimize);
  parameters.push_back(&_sort);
  parameters.push_back(&_unique);
  parameters.push_back(&_deform);
  parameters.push_back(&_radical);
  Action::obtainParameters(parameters);
}

void TransformAction::perform() {
  _io.validateFormats();
  string iformat = _io.getInputFormat();
  string oformat = _io.getOutputFormat();

  IOFacade facade(_printActions);

  BigIdeal ideal;
  facade.readIdeal(stdin, ideal, _io.getInputFormat());

  IdealFacade idealFacade(_printActions);

  if (_radical)
	idealFacade.takeRadical(ideal);

  if (_minimize || _radical) {
	if (_deform)
	  idealFacade.sortAllAndMinimize(ideal);
	else {
	  idealFacade.sortAllAndMinimize(ideal, stdout,
									 _io.getOutputFormat());
	  return;
	}
  }

  if (_canonicalize)
	idealFacade.sortVariables(ideal);

  if (_unique)
	idealFacade.sortGeneratorsUnique(ideal);

  if (_deform)
	idealFacade.deform(ideal);

  if (_sort || _canonicalize)
	idealFacade.sortGenerators(ideal);

  facade.writeIdeal(stdout, ideal, _io.getOutputFormat());
}
