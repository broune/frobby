#ifndef DYNAMIC_FROBENIUS_FACADE_GUARD
#define DYNAMIC_FROBENIUS_FACADE_GUARD

#include "Facade.h"
#include <vector>

class Configuration;

class DynamicFrobeniusFacade : private Facade {
 public:
  DynamicFrobeniusFacade(bool printActions);

  void computeFrobeniusNumber(const vector<mpz_class>& instance,
			      mpz_class& frobeniusNumber);
};

#endif
