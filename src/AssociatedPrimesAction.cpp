#include "stdinc.h"
#include "AssociatedPrimesAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "AssociatedPrimesFacade.h"
#include "IrreducibleDecomParameters.h"

AssociatedPrimesAction::AssociatedPrimesAction():
  _algorithm
("alg",
 "The algorithm used. The only current option is irrdecom.",
 "irrdecom") {
}

const char* AssociatedPrimesAction::getName() const {
  return "assprimes";
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
  //parameters.push_back(&_algorithm); TODO: remove this paramter entirely
  Action::obtainParameters(parameters);
}

void AssociatedPrimesAction::perform() {
  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(stdin, ideal);

  AssociatedPrimesFacade facade(_printActions);

  string alg;
  _algorithm.getValue(alg);
  if (alg == "irrdecom") {
    IrreducibleDecomParameters params;
    facade.computeAPUsingIrrDecom(ideal, params, stdout);
  } else {
    fprintf(stderr, "ERROR: Unknown algorithm parameter \"%s\".\n",
	    alg.c_str());
    exit(1);
  }
}
