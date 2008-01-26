#include "stdinc.h"
#include "IntersectAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IntersectFacade.h"

const char* IntersectAction::getName() const {
  return "intersect";
}

const char* IntersectAction::getShortDescription() const {
  return "Intersect the input ideals.";
}

const char* IntersectAction::getDescription() const {
  return
"Computes the intersection of the input ideals. Simply concatenate the textual\n"
"representations of the ideals in order to intersect them.\n"
    "\n"
    "Note that this operation is currently implemented in a rather slow way.";
}

Action* IntersectAction::createNew() const {
  return new IntersectAction();
}

void IntersectAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _io.obtainParameters(parameters);
}

void IntersectAction::perform() {
  vector<BigIdeal*> ideals;

  _io.validateFormats();

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeals(stdin, ideals, _io.getInputFormat());

  IntersectFacade facade(_printActions);
  BigIdeal* intersection = facade.intersect(ideals);

  ioFacade.writeIdeal(stdout, *intersection, _io.getOutputFormat());

  delete intersection;
  for (size_t i = 0; i < ideals.size(); ++i)
    delete ideals[i];
}
