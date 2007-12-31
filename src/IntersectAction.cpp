#include "stdinc.h"
#include "IntersectAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IntersectFacade.h"

const char* IntersectAction::getName() const {
  return "intersect";
}

const char* IntersectAction::getShortDescription() const {
  return "Intersect monomial ideals.";
}

const char* IntersectAction::getDescription() const {
  return
    "Computes the intersection of the ideals in the input. These ideals\n"
    "must be represented as the concatenation of ideals, where the\n"
    "representation of each ideal must list the same variables in the\n"
    "same order.\n"
    "\n"
    "Do note that this operation is currently implemented in a slow way.\n";
}

Action* IntersectAction::createNew() const {
  return new IntersectAction();
}

void IntersectAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
}

void IntersectAction::perform() {
  vector<BigIdeal*> ideals;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeals(stdin, ideals);
  /*
  if (ideals.empty()) {
    fputs("ERROR: intersection requires at least one ideal.\n", stderr);
    exit(1);
	}*/

  IntersectFacade facade(_printActions);
  BigIdeal* intersection = facade.intersect(ideals);
  
  ioFacade.writeIdeal(stdout, *intersection);

  delete intersection;
  for (size_t i = 0; i < ideals.size(); ++i)
    delete ideals[i];
}
