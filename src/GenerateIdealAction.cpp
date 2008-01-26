#include "stdinc.h"
#include "GenerateIdealAction.h"

#include "BigIdeal.h"
#include "GenerateDataFacade.h"
#include "IOFacade.h"

GenerateIdealAction::GenerateIdealAction():
  _type
("type",
 "The supported type of ideals are random, list, knight and king.",
 "random"),
  _variableCount("varCount", "The number of variables.", 3),
  _generatorCount("genCount", "The number of minimal generators.", 5),
  _exponentRange
  ("expRange",
   "Exponents are chosen uniformly in the range [0,INTEGER].", 9),
  _io(IOParameters::OutputOnly) {
}

const char* GenerateIdealAction::getName() const {
  return "genideal";
}

const char* GenerateIdealAction::getShortDescription() const {
  return "Generate a random monomial ideal.";
}

const char* GenerateIdealAction::getDescription() const {
  return
"Generate a monomial ideal. The default is to generate a random one,\n"
"and in certain rare cases the generated ideal may have less than the requested\n"
"number of minimial generators. This can be alleviated by increasing the\n"
"exponent range or the number of variables.\n"
"\n"
"The other types of ideals are not random, and they use the number of variables\n"
"as a parameter and may thus have more generators, depending on the type.";
}

Action* GenerateIdealAction::createNew() const {
  return new GenerateIdealAction();
}

void GenerateIdealAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _io.obtainParameters(parameters);

  parameters.push_back(&_exponentRange);
  parameters.push_back(&_variableCount);
  parameters.push_back(&_generatorCount);
  parameters.push_back(&_type);
}

void GenerateIdealAction::perform() {
  BigIdeal ideal;

  _io.validateFormats();

  GenerateDataFacade generator(_printActions);
  const string& type = _type;
  if (type == "random")
	generator.generateIdeal(ideal, 
							_exponentRange,
							_variableCount,
							_generatorCount);
  else if (type == "list")
	generator.generateListIdeal(ideal, _variableCount);
  else if (type == "king")
	generator.generateKingChessIdeal(ideal, _variableCount);
  else if (type == "knight")
	generator.generateKnightChessIdeal(ideal, _variableCount);
  else {
	fprintf(stderr, "ERROR: Unknown ideal type \"%s\".", type.c_str());
	exit(1);
  }

  IOFacade ioFacade(_printActions);
  ioFacade.writeIdeal(stdout, ideal, _io.getOutputFormat());
}
