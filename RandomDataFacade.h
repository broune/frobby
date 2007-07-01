#ifndef RANDOM_DATA_FACADE_GUARD
#define RANDOM_DATA_FACADE_GUARD

#include "Facade.h"

class BigIdeal;
class GenerateIdealParameters;

class RandomDataFacade : private Facade {
 public:
  RandomDataFacade(bool printActions);

  void generateIdeal(BigIdeal& ideal,
		     const GenerateIdealParameters& generateIdealParams);
  void generateFrobeniusInstance(vector<mpz_class>& instance);
};

#endif
