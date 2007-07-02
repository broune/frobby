#include "stdinc.h"
#include "IrreducibleDecomAction.h"

#include "BigIdeal.h"
#include "IrreducibleDecomFacade.h"
#include "IOFacade.h"

const char* IrreducibleDecomAction::getName() const {
  return "irrdecom";
}

const char* IrreducibleDecomAction::getShortDescription() const {
  return "Compute the irreducible decomposition of the input ideal.";
}

const char* IrreducibleDecomAction::getDescription() const {
  return
"Compute the irredundant irreducible decomposition of the input monomial ideal.\n"
"Note that the input generating set MUST be minimal to ensure correct answers.\n"
"\n"
"The decomposition is computed using the Label Algorithm. This algorithm is\n"
"described in the paper `The Label Algorithm For Irreducible Decomposition of\n"
"Monomial Ideals', which is available at http://www.broune.com/ .";
}

Action* IrreducibleDecomAction::createNew() const {
  return new IrreducibleDecomAction();
}

void IrreducibleDecomAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _decomParameters.obtainParameters(parameters);
}

void IrreducibleDecomAction::perform() {
  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(cin, ideal);

  IrreducibleDecomFacade facade(_printActions, _decomParameters);
  facade.computeIrreducibleDecom(ideal, cout);
}
