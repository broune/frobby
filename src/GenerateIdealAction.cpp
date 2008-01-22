#include "stdinc.h"
#include "GenerateIdealAction.h"

#include "BigIdeal.h"
#include "GenerateDataFacade.h"
#include "IOFacade.h"

GenerateIdealAction::GenerateIdealAction():
  _type("type", "The type of ideal to generate.", "random"),
  _variableCount("varCount", "The number of variables.", 3),
  _generatorCount("genCount", "The number of minimal generators.", 5),
  _exponentRange
  ("expRange",
   "Exponents are chosen uniformly in the range [0,INTEGER].", 9) {
}

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
  parameters.push_back(&_exponentRange);
  parameters.push_back(&_variableCount);
  parameters.push_back(&_generatorCount);
  parameters.push_back(&_type);
  Action::obtainParameters(parameters);
}

void GenerateIdealAction::perform() {
  BigIdeal ideal;

  GenerateDataFacade generator(_printActions);
  const string& type = _type;
  if (type == "random") {
	generator.generateIdeal(ideal, 
							_exponentRange,
							_variableCount,
							_generatorCount);
  } else if (type == "list") {
	generator.generateListIdeal(ideal, _variableCount);
  } else {
	fprintf(stderr, "ERROR: Unknown ideal type \"%s\".", type.c_str());
	exit(1);
  }

  IOFacade ioFacade(_printActions);
  ioFacade.writeIdeal(stdout, ideal);
}
