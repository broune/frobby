#include "stdinc.h"
#include "PrimaryDecomFacade.h"

#include "BigIdeal.h"
#include "IOFacade.h"

PrimaryDecomFacade::PrimaryDecomFacade(bool printActions):
  Facade(printActions) {
}

void PrimaryDecomFacade::
computePrimaryDecom(BigIdeal& ideal, FILE* out) {
  beginAction("Computing primary decomposition using the Uwe algorithm.\n");

  cerr << "ERROR: not implemented" << endl;
  ASSERT(false);
  exit(1);
  

  vector<BigIdeal*> ideals;
  //  ::computePrimaryDecom(ideal, ideals);

  endAction();

  IOFacade ioFacade(isPrintingActions());
  for (size_t i = 0; i < ideals.size(); ++i) {
    ioFacade.writeIdeal(out, *(ideals[i]));
    delete ideals[i];
  }
}
