#include "stdinc.h"
#include "AnalyzeAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"
#include "IrreducibleDecomParameters.h"
#include "IrreducibleDecomFacade.h"
#include "Scanner.h"

AnalyzeAction::AnalyzeAction():
  _printLcm
  ("lcm",
   "Print the least common multiple of the generators.",
   false),
  
  _printLabels
  ("label",
   "Print the irreducible decomposition along with labels.",
   false) {
}

const char* AnalyzeAction::getName() const {
  return "analyze";
}

const char* AnalyzeAction::getShortDescription() const {
  return "Display information about the input ideal.";
}

const char* AnalyzeAction::getDescription() const {
  return "Compute and display information about input ideal.";
}

Action* AnalyzeAction::createNew() const {
  return new AnalyzeAction();
}

void AnalyzeAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _io.obtainParameters(parameters);

  parameters.push_back(&_printLcm);
  parameters.push_back(&_printLabels);
}

void AnalyzeAction::perform() {
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(in, ideal);

  IdealFacade idealFacade(_printActions);
  if (_printLcm)
	idealFacade.printLcm(stdout, ideal);
  if (_printLabels) {
	IrreducibleDecomParameters params;
	IrreducibleDecomFacade irrFacade(_printActions, params);
	irrFacade.printLabels(ideal, stdout, _io.getOutputFormat());
  }

  idealFacade.printAnalysis(stderr, ideal);
}
