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
#ifndef BIG_POLYNOMIAL_GUARD
#define BIG_POLYNOMIAL_GUARD

class VarNames;
class Term;
class TermTranslator;

#include "VarNames.h"
#include <vector>

class BigPolynomial {
 public:
  BigPolynomial(const VarNames& names);

  size_t getTermCount() const;
  size_t getVarCount() const;
  const VarNames& getNames() const;

  const mpz_class& getCoef(size_t index) const;
  const vector<mpz_class>& getTerm(size_t index) const;

  void add(const mpz_class& coef, const vector<mpz_class> term);
  void add(const mpz_class& coef, mpz_ptr* term);
  void add(const mpz_class& coef,
		   const Term& term,
		   TermTranslator* translator);

 private:
  struct BigCoefTerm {
	mpz_class coef;
	vector<mpz_class> term;
  };
  typedef vector<BigCoefTerm>::iterator iterator;
  typedef vector<BigCoefTerm>::const_iterator const_iterator;

  VarNames _names;
  vector<BigCoefTerm> _coefTerms;
};

#endif