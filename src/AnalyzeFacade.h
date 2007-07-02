#ifndef ANALYZE_FACADE_GUARD
#define ANALYZE_FACADE_GUARD

#include "Facade.h"

class BigIdeal;

class AnalyzeFacade : private Facade {
 public:
  AnalyzeFacade(bool printActions);

  void printAnalysis(ostream& out, BigIdeal& ideal);
};

#endif
