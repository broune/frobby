#include "stdinc.h"
#include "AssociatedPrimesAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "AssociatedPrimesFacade.h"
#include "IrreducibleDecomParameters.h"

AssociatedPrimesAction::AssociatedPrimesAction():
  _algorithm
("alg",
 "The algorithm used. Either uwe or irrdecom.",
 "irrdecom") {
}

const char* AssociatedPrimesAction::getName() const {
  return "assprimes";
}

const char* AssociatedPrimesAction::getShortDescription() const {
  return "Compute the primes associated to a monomial ideal.";
}

const char* AssociatedPrimesAction::getDescription() const {
  return
    "Computes the associated prime ideals of the input monomial ideal. The\n"
    "output is represented as a squarefree monomial ideal. Each minimal\n"
    "generator of this square free ideal represents the associated prime\n"
    "that it generates.";
}

Action* AssociatedPrimesAction::createNew() const {
  return new AssociatedPrimesAction();
}

void AssociatedPrimesAction::obtainParameters(vector<Parameter*>& parameters) {
  parameters.push_back(&_algorithm);
  Action::obtainParameters(parameters);
}

void AssociatedPrimesAction::perform() {
  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(cin, ideal);

  AssociatedPrimesFacade facade(_printActions);

  string alg;
  _algorithm.getValue(alg);
  if (alg == "irrdecom") {
    IrreducibleDecomParameters params;
    facade.computeAPUsingIrrDecom(ideal, params, cout);
  } else if (alg == "uwe") {
    facade.computeAPUsingUwe(ideal, cout);
  } else {
    cout << "ERROR: Unknown algorithm parameter \"" << alg << "\"." << endl;
    exit(1);
  }
}
