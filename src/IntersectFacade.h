#ifndef INTERSECT_FACADE_GUARD
#define INTERSECT_FACADE_GUARD

#include <vector>
#include "Facade.h"

class BigIdeal;

class IntersectFacade : Facade {
 public:
  IntersectFacade(bool printActions);

  // Ideals must not be empty.
  BigIdeal* intersect(const vector<BigIdeal*>& ideals);
};

#endif
