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

#ifndef MY_CONSUMERS_GUARD
#define MY_CONSUMERS_GUARD

#include <gmp.h>
#include "../src/frobby.h"
#include "MyIdeal.h"
#include "MyPolynomial.h"

// A common base class for consumers with some useful common
// functionality.
class MyConsumer {
 protected:
  MyConsumer();

  MyPP toPP(mpz_ptr* exponentVector);
  int toInt(const mpz_t bigInteger);

  size_t _varCount;
};

class MyIdealConsumer : public Frobby::IdealConsumer,
						public MyConsumer {
public:
  MyIdealConsumer();
  
  virtual void idealBegin(size_t varCount);
  virtual void consume(mpz_ptr* exponentVector);
  const MyIdeal& getIdeal() const;
  bool hasAnyOutput() const;

private:
  bool _hasAnyOutput;
  MyIdeal _ideal;
};

class MyIdealsConsumer : public Frobby::IdealConsumer,
						 public MyConsumer {
public:
  MyIdealsConsumer();

  virtual void idealBegin(size_t varCount);
  virtual void consume(mpz_ptr* exponentVector);
  const MyIdeals& getIdeals() const;

private:
  MyIdeals _ideals;
};

class MyPolynomialConsumer : public Frobby::PolynomialConsumer,
							 public MyConsumer {
 public:
  virtual void polynomialBegin(size_t varCount);
  virtual void consume(const mpz_t coef, mpz_ptr* exponentVector);
  const MyPolynomial& getPolynomial() const;

 private:
  MyPolynomial _polynomial;
};

#endif
