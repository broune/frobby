#include "stdinc.h"
#include "GenerateIdealAction.h"

#include "BigIdeal.h"
#include "RandomDataFacade.h"
#include "IOFacade.h"

const char* GenerateIdealAction::getName() const {
  return "genideal";
}

const char* GenerateIdealAction::getShortDescription() const {
  return "Generate a random monomial ideal.";
}

const char* GenerateIdealAction::getDescription() const {
  return
    "Generate a random monomial ideal. The generated ideal may have less\n"
    "than the requested number of minimial generators.";
}

Action* GenerateIdealAction::createNew() const {
  return new GenerateIdealAction();
}

void GenerateIdealAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _parameters.obtainParameters(parameters);
}

void GenerateIdealAction::perform() {
  BigIdeal ideal;
  
  RandomDataFacade randomDataFacade(_printActions);
  randomDataFacade.generateIdeal(ideal, _parameters);

  IOFacade ioFacade(_printActions);
  ioFacade.writeIdeal(cout, ideal);
}
