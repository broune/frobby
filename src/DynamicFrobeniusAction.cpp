#include "stdinc.h"
#include "DynamicFrobeniusAction.h"

#include "IOFacade.h"
#include "DynamicFrobeniusFacade.h"

const char* DynamicFrobeniusAction::getName() const {
  return "frobdyn";
}

const char* DynamicFrobeniusAction::getShortDescription() const {
  return "Compute Frobenius number using a dynamic programming algorithm.";
}

const char* DynamicFrobeniusAction::getDescription() const {
  return
"Compute the Frobenius number of the input Frobenius instance using a simple\n"
"and quite slow dynamic programming algorithm. This functionality has mainly\n"
"been implemented to check the answers of the Grobner basis-based Frobenius\n"
"solver.";
}

Action* DynamicFrobeniusAction::createNew() const {
  return new DynamicFrobeniusAction();
}

void DynamicFrobeniusAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
}

void DynamicFrobeniusAction::perform() {
  vector<mpz_class> instance;

  IOFacade ioFacade(_printActions);
  ioFacade.readFrobeniusInstance(cin, instance);

  mpz_class frobeniusNumber;

  DynamicFrobeniusFacade frobeniusFacade(_printActions);
  frobeniusFacade.computeFrobeniusNumber(instance, frobeniusNumber);

  cout << frobeniusNumber << endl;
}
