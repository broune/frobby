/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "Deformer.h"

#include "Term.h"
#include "TermPredicate.h"
#include "Ideal.h"
#include "TermPredicate.h"
#include "error.h"
#include "IdealOrderer.h"
#include <map>

namespace {
  void deform(Ideal& ideal,
              vector<Exponent>& undeform,
              size_t var,
              bool stronglyGeneric) {
    ASSERT(undeform.empty());

    map<Exponent, vector<Exponent*> > exps;

    Ideal::const_iterator end = ideal.end();
    for (Ideal::const_iterator it = ideal.begin(); it != end; ++it) {
      Exponent& e = (*it)[var];
      if (e == 0)
        continue;
      exps[e].push_back(*it);
    }

    Term tmp(ideal.getVarCount());
    undeform.push_back(0); // zero always maps to zero
    for (map<Exponent, vector<Exponent*> >::iterator it = exps.begin();
         it != exps.end(); ++it) {
      vector<Exponent*>& block = it->second;

      typedef vector<Exponent*>::iterator BlockIt;
      if (stronglyGeneric) {
        for (BlockIt blockIt = block.begin(); blockIt != block.end();
          ++blockIt) {
          undeform.push_back((*blockIt)[var]);
          (*blockIt)[var] = undeform.size() - 1;
        }
      } else {
        undeform.push_back(block.front()[var]);
        Exponent sharedDeformedExponent = undeform.size() - 1;

        for (BlockIt blockIt = block.begin(); blockIt != block.end();
          ++blockIt) {
          bool canUseShared = true;
          for (BlockIt other = blockIt + 1; other != block.end(); ++other) {
            tmp.lcm(*blockIt, *other);
            if (!ideal.strictlyContains(tmp)) {
              canUseShared = false;
              break;
            }
          }
          if (canUseShared)
            (*blockIt)[var] = sharedDeformedExponent;
          else {
            undeform.push_back((*blockIt)[var]);
            (*blockIt)[var] = undeform.size() - 1;
          }
        }
      }
    }
  }
}

Deformer::Deformer(Ideal& ideal,
                   const IdealOrderer& orderer,
                   bool stronglyGeneric):
  _undeform(ideal.getVarCount()) {

  orderer.order(ideal);

  for (size_t var = 0; var < ideal.getVarCount(); ++var)
    deform(ideal, _undeform[var], var, stronglyGeneric);

  ASSERT(!stronglyGeneric || ideal.isStronglyGeneric());
  ASSERT(ideal.isWeaklyGeneric());
}

void Deformer::undeform(Term& term) const {
  ASSERT(term.getVarCount() == _undeform.size());

  for (size_t var = 0; var < term.getVarCount(); ++var) {
    ASSERT(term[var] < _undeform[var].size());
    term[var] = _undeform[var][term[var]];
  }
}
