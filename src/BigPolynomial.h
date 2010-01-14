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
  BigPolynomial();
  BigPolynomial(const VarNames& names);

  size_t getTermCount() const;
  size_t getVarCount() const;
  const VarNames& getNames() const;

  // Removes all terms but does not change getNames().
  void clear();
  void clearAndSetNames(const VarNames& names);

  void sortTermsReverseLex();
  void sortVariables();

  const mpz_class& getCoef(size_t index) const;
  const vector<mpz_class>& getTerm(size_t index) const;

  void newLastTerm();
  vector<mpz_class>& getLastTerm();
  mpz_class& getLastCoef();

  void renameVars(const VarNames& names);

  void add(const mpz_class& coef, const vector<mpz_class> term);
  void add(const mpz_class& coef, const Term& term,
           const TermTranslator& translator);

  bool operator==(const BigPolynomial& poly) const;

  void print(FILE* file) const;
  void print(ostream& out) const;

 private:
  struct BigCoefTerm {
    mpz_class coef;
    vector<mpz_class> term;

    bool operator==(const BigCoefTerm& coefTerm) const;
  };
  typedef vector<BigCoefTerm>::iterator iterator;
  typedef vector<BigCoefTerm>::const_iterator const_iterator;

  static bool compareCoefTermsReverseLex
    (const BigCoefTerm& a, const BigCoefTerm& b);

  VarNames _names;
  vector<BigCoefTerm> _coefTerms;
};

ostream& operator<<(ostream& out, const BigPolynomial& ideal);

#endif
