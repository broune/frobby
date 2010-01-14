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
#ifndef LIB_TEST_GUARD
#define LIB_TEST_GUARD

#include "frobby.h"
#include "BigIdeal.h"
#include "BigPolynomial.h"

#include <vector>

/** Returns a library interface ideal that corresponds to the
 parameter ideal.
*/
Frobby::Ideal toLibIdeal(const BigIdeal& ideal);

/** Returns a pointer to an array of mpt_t that represents the entries
 in the mpz_class. The array is actually just the memory of the
 vector, so the life time of the array is until the vector deallocates
 or reallocates its array.
*/
const mpz_t* castLibArray(const vector<mpz_class>& vect);

/** Records library interface output into a BigIdeal. */
class LibIdealConsumer : public Frobby::IdealConsumer {
 public:
  LibIdealConsumer(const VarNames& names);

  virtual void idealBegin(size_t varCount);
  virtual void consume(mpz_ptr* exponentVector);

  /** Returns the recorded ideal in a canonical form. */
  const BigIdeal& getIdeal() const;

  /** Returns whether any ideal has been recorded. */
  bool hasAnyOutput() const;

private:
  bool _hasAnyOutput;
  mutable BigIdeal _ideal;
};

/** Records library interface output into a vector of BigIdeals. */
class LibIdealsConsumer : public Frobby::IdealConsumer {
public:
  LibIdealsConsumer(const VarNames& names);

  virtual void idealBegin(size_t varCount);
  virtual void consume(mpz_ptr* exponentVector);

  /** Returns the recorded ideals in a canonical form. */
  const vector<BigIdeal>& getIdeals() const;

private:
  mutable vector<BigIdeal> _ideals;
  VarNames _names;
};

/** Records library interface output into a BigPolynomial. */
class LibPolynomialConsumer : public Frobby::PolynomialConsumer {
 public:
  LibPolynomialConsumer(const VarNames& names);

  virtual void polynomialBegin(size_t varCount);
  virtual void consume(const mpz_t coef, mpz_ptr* exponentVector);

  /** Returns the recorded polynomial in a canonical form. */
  const BigPolynomial& getPolynomial() const;

 private:
  mutable BigPolynomial _polynomial;
};

#endif
