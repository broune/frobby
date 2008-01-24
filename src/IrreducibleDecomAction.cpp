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
"Compute the irredundant irreducible decomposition of the input monomial ideal.\n\n"
"The decomposition is computed using the Slice Algorithm. This algorithm is\n"
"described in the paper `The Slice Algorithm For Irreducible Decomposition of\n"
"Monomial Ideals', which is available at www.broune.com . It is also possible\n""to use the older Label algorithm, though this is not as efficient.";
}

Action* IrreducibleDecomAction::createNew() const {
  return new IrreducibleDecomAction();
}

void IrreducibleDecomAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _decomParameters.obtainParameters(parameters);
  _io.obtainParameters(parameters);
}

void IrreducibleDecomAction::perform() {
  BigIdeal ideal;

  _io.validateFormats();
  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(stdin, ideal, _io.getInputFormat().c_str());

  IrreducibleDecomFacade facade(_printActions, _decomParameters);
  facade.computeIrreducibleDecom(ideal, stdout, _io.getOutputFormat().c_str());
}
