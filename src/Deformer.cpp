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
#include "Ideal.h"
#include "error.h"

namespace {
  void deform(Ideal& ideal, vector<Exponent>& undeform, size_t var) {
	ASSERT(undeform.empty());

	ideal.singleDegreeSort(var);
	undeform.push_back(0); // zero always maps to zero

	Ideal::const_iterator end = ideal.end();
	for (Ideal::const_iterator it = ideal.begin(); it != end; ++it) {
	  Exponent& e = (*it)[var];
	  if (e != undeform.back()) {
		if (undeform.size() >= numeric_limits<Exponent>::max())
		  reportError("Cannot deform ideal due to exponent overflow.");
		undeform.push_back(e);
	  }
	  e = undeform.size() - 1;
	}
  }
}

Deformer::Deformer(Ideal& ideal):
  _undeform(ideal.getVarCount()) {
  for (size_t var = 0; var < ideal.getVarCount(); ++var)
	deform(ideal, _undeform[var], var);
}

void Deformer::undeform(Term& term) const {
  ASSERT(term.getVarCount() == _undeform.size());

  for (size_t var = 0; var < term.getVarCount(); ++var) {
	ASSERT(term[var] < _undeform[var].size());
	term[var] = _undeform[var][term[var]];
  }
}
