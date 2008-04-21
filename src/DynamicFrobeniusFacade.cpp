#include "stdinc.h"
#include "DynamicFrobeniusFacade.h"
#include "dynamicFrobeniusAlgorithm.h"

DynamicFrobeniusFacade::
DynamicFrobeniusFacade(bool printActions):
  Facade(printActions) {
}

void DynamicFrobeniusFacade::
computeFrobeniusNumber(const vector<mpz_class>& instance,
		       mpz_class& frobeniusNumber) {
  beginAction
    ("Computing Frobenius number using dynamic programming algorithm.");

  frobeniusNumber = dynamicFrobeniusAlgorithm(instance);

  endAction();
}
