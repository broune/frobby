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
    "Generate a random monomial ideal. In certain rare cases the generated ideal\n"
"may have less than the requested number of minimial generators. This can be\n"
"alleviated by increasing the exponent range or the number of variables.";
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
  ioFacade.writeIdeal(stdout, ideal);
}
