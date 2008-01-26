#ifndef IDEAL_FACADE_GUARD
#define IDEAL_FACADE_GUARD

#include "Facade.h"

class BigIdeal;

class IdealFacade : private Facade {
 public:
  IdealFacade(bool printActions);

  // Applies some generic deformation to the ideal.
  void deform(BigIdeal& ideal);

  // Takes the radical of the ideal and minimizes it.
  void takeRadical(BigIdeal& ideal);

  // Removes redundant generators from ideal.
  void sortAllAndMinimize(BigIdeal& bigIdeal);

  // Clears the input ideal and writes to file.
  void sortAllAndMinimize(BigIdeal& bigIdeal, FILE* out, const string& format);

  // Sorts the generators of ideal and removes duplicates.
  void sortGeneratorsUnique(BigIdeal& ideal);

  // Sorts the generators of ideal.
  void sortGenerators(BigIdeal& ideal);

  // Sorts the variables of ideal.
  void sortVariables(BigIdeal& ideal);


  void printAnalysis(FILE* out, BigIdeal& ideal);
  void printLcm(FILE* out, BigIdeal& ideal);
};

#endif
