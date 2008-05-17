/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "IntersectFacade.h"

#include "Term.h"
#include "BigIdeal.h"
#include "Ideal.h"
#include "intersect.h"
#include "Ideal.h"
#include "TermTranslator.h"

IntersectFacade::IntersectFacade(bool printActions):
  Facade(printActions) {
}

BigIdeal* IntersectFacade::intersect(const vector<BigIdeal*>& ideals) {
  beginAction("Intersecting ideals.");

  //  ASSERT(!ideals.empty());

  if (ideals.empty()) {
	BigIdeal* entireRing = new BigIdeal();
	entireRing->newLastTerm();
	return entireRing;
  }

  if (ideals.size() == 1)
    return new BigIdeal(*ideals[0]);

  vector<Ideal*> ideals2;
  TermTranslator translator(ideals, ideals2);

  const VarNames& names = translator.getNames();
  size_t variableCount = names.getVarCount();

  Ideal* intersection = new Ideal(variableCount);
  Term identity(variableCount);
  intersection->insert(identity);

  for (size_t i = 0; i < ideals2.size(); ++i) {
    // Compute intersection
    Ideal* tmp = new Ideal(variableCount);
    ::intersect(tmp, intersection, ideals2[i]);

    // Handle bookkeeping
    delete ideals2[i];
    delete intersection;
    intersection = tmp;
  }

  BigIdeal* bigIdeal = new BigIdeal(names);
  bigIdeal->insert(*intersection, translator);
  delete intersection;

  endAction();
  return bigIdeal;
}
