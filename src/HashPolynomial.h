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

class CoefBigTermConsumer;
class TermTranslator;

/** This template specialization makes the hash code of a term
 available to the implementation of HashMap.
*/
template<>
class FrobbyHash<Term> {
 public:
  size_t operator()(const Term& t) const {
    return t.getHashCode();
  }
};

/** A sparse multivariate polynomial represented by a hash table
 mapping terms to coefficients. This allows to avoid duplicate terms
 without a large overhead.
*/
class HashPolynomial {
 public:
  HashPolynomial(size_t varCount = 0);

  void clearAndSetVarCount(size_t varCount);

  /** Add coef*term to the polynomial. */
  void add(const mpz_class& coef, const Term& term);

  /** Add +term or -term to the polynomial depending on whether plus
   is true or false, respectively. */
  void add(bool plus, const Term& term);

  void feedTo(const TermTranslator& translator,
              CoefBigTermConsumer& consumer,
              bool inCanonicalOrder) const;

  size_t getTermCount() const;

 private:
  size_t _varCount;

  typedef HashMap<Term, mpz_class> TermMap;
  TermMap _terms;
};

#endif
