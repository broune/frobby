#include "stdinc.h"
#include "LatticeFacade.h"

#include "lattice.h"

LatticeFacade::LatticeFacade(bool printActions):
  Facade(printActions) {
}

void LatticeFacade::makeZeroesInLatticeBasis(BigIdeal& basis) {
  beginAction("Adjusting lattice basis to increse number of zero entries.");

  ::makeZeroesInLatticeBasis(basis);

  endAction();
}
