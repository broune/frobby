#include "stdinc.h"
#include "FrobeniusAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IrreducibleDecomFacade.h"

FrobeniusAction::FrobeniusAction() {
  _decomParameters.setSkipRedundant(false);
}

const char* FrobeniusAction::getName() const {
  return "frobgrob";
}

const char* FrobeniusAction::getShortDescription() const {
  return "Compute Frobenius number using a Grobner basis algorithm.";
}

const char* FrobeniusAction::getDescription() const {
  return
"Compute the Frobenius number of the passed-in Frobenius instance. This instance\n"
"must be preceded in the input by a deg-rev-lex lattice ideal Grobner basis as\n"
"produced by the program 4ti2.\n"
"\n"
"The algorithm for this uses irreducible decomposition to compute the Frobenius\n"
"number, which is why this action accepts parameters related to that. See the\n"
"paper \"Solving Thousand Digit Frobenius Problems Using Grobner Bases\"\n"
"at www.broune.com for more details.";
}

Action* FrobeniusAction::createNew() const {
  return new FrobeniusAction();
}

void FrobeniusAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _decomParameters.obtainParameters(parameters);
}

void FrobeniusAction::perform() {
  vector<mpz_class> instance;
  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  ioFacade.readFrobeniusInstanceWithGrobnerBasis(stdin, ideal, instance);

  IrreducibleDecomFacade facade(_printActions, _decomParameters);

  mpz_class frobeniusNumber;
  facade.computeFrobeniusNumber(instance, ideal, frobeniusNumber);

  gmp_fprintf(stdout, "%Zd\n", frobeniusNumber.get_mpz_t());
}
