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
#include "stdinc.h"
#include "MyConsumers.h"

#include <algorithm>

MyConsumer::MyConsumer():
  _varCount(0) {
}

MyPP MyConsumer::toPP(mpz_ptr* exponentVector) {
  MyPP pp(_varCount);
  for (size_t var = 0; var < _varCount; ++var)
	pp[var] = toInt(exponentVector[var]);
  return pp;
}

int MyConsumer::toInt(const mpz_t bigInteger) {
  if (!mpz_fits_sint_p(bigInteger))
	return 0xFFFFFFFF; // A real program should report an error here.
  else
	  return mpz_get_si(bigInteger);
}

MyIdealConsumer::MyIdealConsumer():
  _hasAnyOutput(false) {
}

void MyIdealConsumer::idealBegin(size_t varCount) {
  _varCount = varCount;
  _hasAnyOutput = true;
}

void MyIdealConsumer::consume(mpz_ptr* exponentVector) {
  _ideal.push_back(toPP(exponentVector));
}

const MyIdeal& MyIdealConsumer::getIdeal() const {
  return _ideal;
}

bool MyIdealConsumer::hasAnyOutput() const {
  return _hasAnyOutput;
}



MyIdealConsumer2::MyIdealConsumer2(const VarNames& names):
  _hasAnyOutput(false),
  _ideal(names) {
}

void MyIdealConsumer2::idealBegin(size_t varCount) {
  _varCount = varCount;
  _hasAnyOutput = true;
}

void MyIdealConsumer2::consume(mpz_ptr* exponentVector) {
  _ideal.newLastTerm();
  for (size_t var = 0; var < _varCount; ++var)
	_ideal.getLastTermRef()[var] = mpz_class(exponentVector[var]);
}

const BigIdeal& MyIdealConsumer2::getIdeal() const {
  _ideal.sortGenerators();
  return _ideal;
}

bool MyIdealConsumer2::hasAnyOutput() const {
  return _hasAnyOutput;
}

  
MyIdealsConsumer::MyIdealsConsumer() {
}

void MyIdealsConsumer::idealBegin(size_t varCount) {
  _varCount = varCount;
  _ideals.resize(_ideals.size() + 1);
}

void MyIdealsConsumer::consume(mpz_ptr* exponentVector) {
  _ideals.back().push_back(toPP(exponentVector));
}

const MyIdeals& MyIdealsConsumer::getIdeals() const {
  return _ideals;
}





MyIdealsConsumer2::MyIdealsConsumer2(const VarNames& names):
  _names(names) {
}

void MyIdealsConsumer2::idealBegin(size_t varCount) {
  _varCount = varCount;
  _ideals.push_back(BigIdeal(_names));
}

void MyIdealsConsumer2::consume(mpz_ptr* exponentVector) {
  _ideals.back().newLastTerm();
  for (size_t var = 0; var < _varCount; ++var)
	_ideals.back().getLastTermRef()[var] = mpz_class(exponentVector[var]);
}

const vector<BigIdeal>& MyIdealsConsumer2::getIdeals() const {
  for (size_t i = 0; i < _ideals.size(); ++i)
	_ideals[i].sortGenerators();
  sort(_ideals.begin(), _ideals.end());
  return _ideals;
}






void MyPolynomialConsumer::polynomialBegin(size_t varCount) {
  _varCount = varCount;
}

void MyPolynomialConsumer::consume(const mpz_t coef, mpz_ptr* exponentVector) {
  _polynomial.push_back(MyTerm(toInt(coef), toPP(exponentVector)));
}

const MyPolynomial& MyPolynomialConsumer::getPolynomial() const {
  return _polynomial;
}
