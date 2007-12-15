#include "stdinc.h"
#include "PrimaryDecomAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "PrimaryDecomFacade.h"

const char* PrimaryDecomAction::getName() const {
  return "primdecom";
}

const char* PrimaryDecomAction::getShortDescription() const {
  return "Computes the primary decomposition of monomial ideals.";
}

const char* PrimaryDecomAction::getDescription() const {
  return
    "Computes the primary decomposition of monomial ideals using\n"
    "the Uwe algorithm.\n"
    "\n"
    "Do note that this operation is currently implemented in a slow way.\n";
}

Action* PrimaryDecomAction::createNew() const {
  return new PrimaryDecomAction();
}

void PrimaryDecomAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
}

void PrimaryDecomAction::perform() {
  BigIdeal ideal;
  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(stdin, ideal);

  PrimaryDecomFacade facade(_printActions);
  facade.computePrimaryDecom(ideal, stdout);
}
