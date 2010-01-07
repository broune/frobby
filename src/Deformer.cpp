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
#include <map>

namespace {
  void deform(Ideal& ideal, vector<Exponent>& undeform, size_t var) {
	ASSERT(undeform.empty());

	ideal.singleDegreeSort(var);
	undeform.push_back(0); // zero always maps to zero
	
	Ideal::const_iterator end = ideal.end();
	for (Ideal::const_iterator it = ideal.begin(); it != end; ++it) {
	  Exponent& e = (*it)[var];
	  if (e == 0)
		continue;
	  if (numeric_limits<Exponent>::max() < numeric_limits<size_t>::max() &&
		  undeform.size() > numeric_limits<Exponent>::max())
	    reportError("Cannot deform ideal due to exponent overflow.");
	  
	  undeform.push_back(e);
	  e = undeform.size() - 1;
    }
  }

  void deformSort(Ideal& ideal, vector<Exponent>& undeform, size_t var) {
	ASSERT(undeform.empty());

	map<Exponent, vector<Exponent*> > exps;

	Ideal::const_iterator end = ideal.end();
	for (Ideal::const_iterator it = ideal.begin(); it != end; ++it) {
	  Exponent& e = (*it)[var];
	  if (e == 0)
		continue;
	  exps[e].push_back(*it);
	}

	undeform.push_back(0); // zero always maps to zero
	for (map<Exponent, vector<Exponent*> >::iterator it = exps.begin();
		 it != exps.end(); ++it) {
	  for (size_t i = 0; i < it->second.size(); ++i) {
		Exponent& e = (it->second[i])[var];
		undeform.push_back(e);
		e = undeform.size() - 1;
	  }
	}
  }
}

mpz_class tdeg(const Exponent* term, size_t varCount) {
  mpz_class deg = 0;
  for (size_t var = 0; var < varCount; ++var)
	deg += term[var];
  return deg;
}

  class C {
  public:
  C(size_t varCount): _varCount(varCount) {}

    bool operator()(const Exponent* a, const Exponent* b) const {
	  mpz_class da = tdeg(a, _varCount);
	  mpz_class db = tdeg(b, _varCount);
	  if (da < db)
		return true;
	  if (da > db)
		return false;
		  return Term::reverseLexCompare(a,b, _varCount) < 0;
    }

  private:
    size_t _varCount;
  };

Deformer::Deformer(Ideal& ideal):
  _undeform(ideal.getVarCount()) {
  //ideal.sortReverseLex();
  //random_shuffle(ideal.begin(), ideal.end());
  //sort(ideal.begin(), ideal.end(), C(ideal.getVarCount()));
  
  for (size_t var = 0; var < ideal.getVarCount(); ++var)
	deformSort(ideal, _undeform[var], var);
  ideal.sortReverseLex();
}

void Deformer::undeform(Term& term) const {
  ASSERT(term.getVarCount() == _undeform.size());

  for (size_t var = 0; var < term.getVarCount(); ++var) {
	ASSERT(term[var] < _undeform[var].size());
	term[var] = _undeform[var][term[var]];
  }
}
