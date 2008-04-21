#ifndef LATTICE_FACADE_GUARD
#define LATTICE_FACADE_GUARD

#include "Facade.h"
class BigIdeal;

class LatticeFacade : public Facade {
 public:
  LatticeFacade(bool printActions);

  void makeZeroesInLatticeBasis(BigIdeal& basis);
};

#endif
