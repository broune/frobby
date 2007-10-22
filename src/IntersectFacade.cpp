#include "stdinc.h"
#include "IntersectFacade.h"

#include "BigIdeal.h"
#include "Ideal.h"
#include "intersect.h"
#include "TermList.h"
#include "TermTranslator.h"
#include "TermTree.h"

IntersectFacade::IntersectFacade(bool printActions):
  Facade(printActions) {
}

BigIdeal* IntersectFacade::intersect(const vector<BigIdeal*>& ideals) {
  beginAction("Intersecting ideals.");

  ASSERT(!ideals.empty());

  if (ideals.size() == 1)
    return new BigIdeal(*ideals[0]);

  const VarNames& names = ideals[0]->getNames();
  size_t variableCount = names.getVarCount();

  vector<Ideal*> ideals2;
  TermTranslator* translator = BigIdeal::buildAndClear(ideals, ideals2);

  TermList* intersection = new TermList(variableCount);
  Term identity(variableCount);
  intersection->insert(identity);

  for (size_t i = 0; i < ideals2.size(); ++i) {
    ASSERT(names == ideals[i]->getNames());

    // Compute intersection
    TermList* tmp = new TermList(variableCount);
    ::intersect(tmp, intersection, ideals2[i]);

    // Handle bookkeeping
    delete ideals2[i];
    delete intersection;
    intersection = tmp;
  }

  BigIdeal* bigIdeal = new BigIdeal(names);
  bigIdeal->insert(*intersection, *translator);
  delete intersection;
  delete translator;

  endAction();
  return bigIdeal;
}
