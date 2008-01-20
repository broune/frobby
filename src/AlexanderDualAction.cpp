#include "stdinc.h"
#include "AlexanderDualAction.h"

#include "BigIdeal.h"
#include "IrreducibleDecomFacade.h"
#include "IOFacade.h"

const char* AlexanderDualAction::getName() const {
  return "alexdual";
}

const char* AlexanderDualAction::getShortDescription() const {
  return "Compute the Alexander dual of the input ideal.";
}

const char* AlexanderDualAction::getDescription() const {
  return
"Compute the alexander dual of the input monomial ideal.\n"
"\n"
"The computation is done using irreducible decomposition, which is why there are\n"
"a number of options related to that. See the help topic on irrdecom for details.";
}

Action* AlexanderDualAction::createNew() const {
  return new AlexanderDualAction();
}

void AlexanderDualAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _decomParameters.obtainParameters(parameters);
}

void AlexanderDualAction::perform() {
  BigIdeal ideal;
  vector<mpz_class> point;

  IOFacade ioFacade(_printActions);
  bool pointSpecified =
	ioFacade.readAlexanderDualInstance(stdin, ideal, point);

  IrreducibleDecomFacade facade(_printActions, _decomParameters);
  if (pointSpecified)
	facade.computeAlexanderDual(ideal, point, stdout);
  else
	facade.computeAlexanderDual(ideal, stdout);
}
