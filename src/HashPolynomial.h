/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
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
#ifndef HASH_POLYNOMIAL_GUARD
#define HASH_POLYNOMIAL_GUARD

#include "Term.h"
#include "HashMap.h"

class CoefTermConsumer;

template<>
class FrobbyHash<Term> {
 public:
  size_t operator()(const Term& t) const {
	return t.getHashCode();
  }
};

class HashPolynomial {
 public:
  HashPolynomial(size_t varCount = 0);

  void clearAndSetVarCount(size_t varCount);

  void add(const mpz_class& coef, const Term& term);

  void feedTo(CoefTermConsumer& consumer) const;

 private:
  size_t _varCount;

  typedef HashMap<Term, mpz_class> TermMap;
  TermMap _terms;
};

#endif
