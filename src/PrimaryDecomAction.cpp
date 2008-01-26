#include "stdinc.h"
#include "PrimaryDecomAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"

const char* PrimaryDecomAction::getName() const {
  return "primdecom";
}

const char* PrimaryDecomAction::getShortDescription() const {
  return "Computes the primary decomposition of monomial ideals.";
}

const char* PrimaryDecomAction::getDescription() const {
  return "TODO";
}

Action* PrimaryDecomAction::createNew() const {
  return new PrimaryDecomAction();
}

void PrimaryDecomAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
}

void PrimaryDecomAction::perform() {
  // TODO
}
