#ifndef IDEAL_FACADE_GUARD
#define IDEAL_FACADE_GUARD

#include "Facade.h"

class BigIdeal;

class IdealFacade : private Facade {
 public:
  IdealFacade(bool printActions);

  // Removes redundant generators from ideal.
  void minimize(BigIdeal& ideal);

  // Sorts the generators of ideal and removes duplicates.
  void sortGeneratorsUnique(BigIdeal& ideal);

  // Sorts the generators of ideal.
  void sortGenerators(BigIdeal& ideal);

  // Sorts the variables of ideal.
  void sortVariables(BigIdeal& ideal);

  void printAnalysis(FILE* out, BigIdeal& ideal);
};

#endif
