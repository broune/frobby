/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

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
#include "LibTest.h"

#include "tests.h"

#include <algorithm>

TEST_SUITE(LibraryInterface)

Frobby::Ideal toLibIdeal(const BigIdeal& ideal) {
  size_t varCount = ideal.getVarCount();

  Frobby::Ideal libIdeal(varCount);
  for (size_t generator = 0; generator < ideal.getGeneratorCount(); ++generator) {
    if (varCount == 0)
      libIdeal.addExponent(0);
    else
      for (size_t var = 0; var < varCount; ++var)
        libIdeal.addExponent(ideal[generator][var].get_mpz_t());
  }

  return libIdeal;
}

const mpz_t* castLibArray(const vector<mpz_class>& vect) {
  // The following cast depends on knowing that mpz_class just
  // consists of a single mpz_t, so (nearly) assert that that is true.
  ASSERT(sizeof(mpz_class) == sizeof(mpz_t));
  return reinterpret_cast<const mpz_t*>(&(vect[0]));
}

LibIdealConsumer::LibIdealConsumer(const VarNames& names):
  _hasAnyOutput(false),
  _ideal(names) {
}

void LibIdealConsumer::idealBegin(size_t varCount) {
  ASSERT(varCount == _ideal.getVarCount());

  _ideal.clear();
  _hasAnyOutput = true;
}

void LibIdealConsumer::consume(mpz_ptr* exponentVector) {
  _ideal.newLastTerm();
  for (size_t var = 0; var < _ideal.getVarCount(); ++var)
    _ideal.getLastTermRef()[var] = mpz_class(exponentVector[var]);
}

const BigIdeal& LibIdealConsumer::getIdeal() const {
  _ideal.sortGenerators();
  return _ideal;
}

bool LibIdealConsumer::hasAnyOutput() const {
  return _hasAnyOutput;
}


LibIdealsConsumer::LibIdealsConsumer(const VarNames& names):
  _names(names) {
}

void LibIdealsConsumer::idealBegin(size_t varCount) {
  ASSERT(_names.getVarCount() == varCount);
  _ideals.push_back(BigIdeal(_names));
}

void LibIdealsConsumer::consume(mpz_ptr* exponentVector) {
  _ideals.back().newLastTerm();
  for (size_t var = 0; var < _names.getVarCount(); ++var)
    _ideals.back().getLastTermRef()[var] = mpz_class(exponentVector[var]);
}

const vector<BigIdeal>& LibIdealsConsumer::getIdeals() const {
  for (size_t i = 0; i < _ideals.size(); ++i)
    _ideals[i].sortGenerators();
  sort(_ideals.begin(), _ideals.end());
  return _ideals;
}

LibPolynomialConsumer::LibPolynomialConsumer(const VarNames& names):
  _polynomial(names) {
}

void LibPolynomialConsumer::polynomialBegin(size_t varCount) {
  ASSERT(varCount == _polynomial.getVarCount());
}

void LibPolynomialConsumer::consume(const mpz_t coef, mpz_ptr* exponentVector) {
  _polynomial.newLastTerm();
  _polynomial.getLastCoef() = mpz_class(coef);
  for (size_t var = 0; var < _polynomial.getVarCount(); ++var)
    _polynomial.getLastTerm()[var] = mpz_class(exponentVector[var]);
}

const BigPolynomial& LibPolynomialConsumer::getPolynomial() const {
  _polynomial.sortTermsReverseLex();
  return _polynomial;
}
