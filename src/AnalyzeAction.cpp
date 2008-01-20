#include "stdinc.h"
#include "AnalyzeAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"

AnalyzeAction::AnalyzeAction():
  _printLcm
("printLcm",
 "Print the least common multiple of the generators.",
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
  parameters.push_back(&_printLcm);
}

void AnalyzeAction::perform() {
  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(stdin, ideal);

  IdealFacade idealFacade(_printActions);
  if (_printLcm)
	idealFacade.printLcm(stdout, ideal);
  idealFacade.printAnalysis(stderr, ideal);
}
