#include "stdinc.h"
#include "AnalyzeAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"

const char* AnalyzeAction::getName() const {
  return "analyze";
}

const char* AnalyzeAction::getShortDescription() const {
  return "Compute and display information about input ideal.";
}

const char* AnalyzeAction::getDescription() const {
  return "Compute and display information about input ideal.";
}

Action* AnalyzeAction::createNew() const {
  return new AnalyzeAction();
}

void AnalyzeAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
}

void AnalyzeAction::perform() {
  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(stdin, ideal);

  IdealFacade idealFacade(_printActions);
  idealFacade.printAnalysis(cout, ideal);
}
