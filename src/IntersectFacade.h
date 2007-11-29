#ifndef INTERSECT_FACADE_GUARD
#define INTERSECT_FACADE_GUARD

#include <vector>
#include "Facade.h"

class BigIdeal;

class IntersectFacade : Facade {
 public:
  IntersectFacade(bool printActions);

  BigIdeal* intersect(const vector<BigIdeal*>& ideals);
};

#endif
