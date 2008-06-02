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
#ifndef POLYNOMIAL_GUARD
#define POLYNOMIAL_GUARD

#include "Term.h"
#include <vector>

class Polynomial {
 public:
  Polynomial(size_t varCount);

  size_t getVarCount() const {return _varCount;}
  size_t getTermCount() const {return _terms.size();}
  const mpz_class& getCoef(size_t index) const {
	ASSERT(index < getTermCount());
	return _terms[index].coef;
  }
  const Term& getTerm(size_t index) const {
	ASSERT(index < getTermCount());
	return _terms[index].term;
  }

  // Add coef*term to the polynomial. Does not collect terms with the
  // same exponents. Does nothing if coef is zero.
  void add(const mpz_class& coef, const Term& term);

  // Sort the terms in lexicographic order.
  void sortTermsLex();

 private:
  struct CoefTerm {
	bool operator<(const CoefTerm& coefTerm) const;

	mpz_class coef;
	Term term;
  };

  vector<CoefTerm> _terms;
  size_t _varCount;
};

#endif