#include "stdinc.h"
#include "AssociatedPrimesAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "AssociatedPrimesFacade.h"
#include "IrreducibleDecomParameters.h"

AssociatedPrimesAction::AssociatedPrimesAction() {
}

const char* AssociatedPrimesAction::getName() const {
  return "assoprimes";
}

const char* AssociatedPrimesAction::getShortDescription() const {
  return "Compute the associated primes of the input ideal.";
}

const char* AssociatedPrimesAction::getDescription() const {
  return
"Computes the associated prime ideals of the input monomial ideal. The\n"
"computation is accomplished using irreducible decomposition. The quality of the\n"
"algorithm for computing associated primes is expected to be much improved in a\n"
"future version of Frobby.";
}

Action* AssociatedPrimesAction::createNew() const {
  return new AssociatedPrimesAction();
}

void AssociatedPrimesAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _io.obtainParameters(parameters);
}

void AssociatedPrimesAction::perform() {
  BigIdeal ideal;

  _io.validateFormats();

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(stdin, ideal, _io.getInputFormat());

  AssociatedPrimesFacade facade(_printActions);

  facade.computeAPUsingIrrDecom(ideal, _decomParameters, stdout,
								_io.getOutputFormat());
}
