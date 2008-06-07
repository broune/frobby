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
#include "intersect.h"

#include "Ideal.h"
#include "Term.h"

void intersect(Ideal* output, const Ideal* a, const Ideal* b) {
  ASSERT(a->getVarCount() == b->getVarCount());
  output->clear();

  Term term(a->getVarCount());
  for (Ideal::const_iterator aIt = a->begin(); aIt != a->end(); ++aIt) {
    for (Ideal::const_iterator bIt = b->begin(); bIt != b->end();++bIt) {
      term.lcm(*aIt, *bIt);
      output->insert(term);
    }
  }

  output->minimize();
}
