#ifndef GENERATE_DATA_FACADE_GUARD
#define GENERATE_DATA_FACADE_GUARD

#include "Facade.h"
#include <vector>

class BigIdeal;

class GenerateDataFacade : private Facade {
 public:
  GenerateDataFacade(bool printActions);

  void generateListIdeal(BigIdeal& ideal, size_t variableCount);
  void generateKingChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns);
  void generateKnightChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns);

  void generateIdeal(BigIdeal& ideal,
					 size_t exponentRange,
					 size_t variableCount,
					 size_t generatorCount);

  void generateFrobeniusInstance(vector<mpz_class>& instance);
};

#endif
