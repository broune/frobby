/* Frobby, software for computations related to monomial ideals.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/ 
#ifndef IRREDUCIBLE_DECOM_FACADE_GUARD
#define IRREDUCIBLE_DECOM_FACADE_GUARD

#include "Facade.h"
#include <vector>

class IrreducibleDecomParameters;
class BigIdeal;
class Strategy;
class TermTranslator;
class SliceStrategy;
class Ideal;
class TermConsumer;


// TODO: move this elsewhere
// TODO: avoid copying
#include "frobby.h"
#include "TermConsumer.h"
#include "TermTranslator.h"
class ExternalConsumer : public TermConsumer {
 public:
  ExternalConsumer(Frobby::TermConsumer* consumer, TermTranslator* trans):
	_varCount(trans->getNames().getVarCount()),
	_consumer(consumer),
	_translator(trans),
	_exponentVector(new mpz_t[_varCount]) {
	for (size_t var = 0; var < _varCount; ++var)
	  mpz_init(_exponentVector[var]);
  }

  ~ExternalConsumer() {
	for (size_t var = 0; var < _varCount; ++var)
	  mpz_clear(_exponentVector[var]);
	delete[] _exponentVector;
  }

  virtual void consume(const Term& term) {
	for (size_t var = 0; var < _varCount; ++var)
	  mpz_set(_exponentVector[var],
			  _translator->getExponent(var, term).get_mpz_t());
	_consumer->consume((const mpz_t*)_exponentVector);
  }

 private:
  size_t _varCount;															
  Frobby::TermConsumer* _consumer;
  TermTranslator* _translator;
  mpz_t* _exponentVector;
};

class IrreducibleDecomFacade : private Facade {
 public:
  IrreducibleDecomFacade(bool printActions,
						 const IrreducibleDecomParameters& parameters);

  void printLabels(BigIdeal& ideal, FILE* out, const string& format);

  // These all clear ideal.
  void computeIrreducibleDecom(BigIdeal& ideal, FILE* out, const string& format);
  void computeAlexanderDual(BigIdeal& ideal, FILE* out, const string& format);
  void computeAlexanderDual(BigIdeal& ideal,
							const vector<mpz_class>& point, FILE* out,
							const string& format);
  void computeAlexanderDual(BigIdeal& ideal,
							const vector<mpz_class>& point,
							Frobby::TermConsumer* consumer);

  void computeFrobeniusNumber(const vector<mpz_class>& instance,
							  BigIdeal& ideal, 
							  mpz_class& frobeniusNumber,
							  vector<mpz_class>& vector);

  void computeIrreducibleDecom(Ideal& ideal, TermConsumer* consumer);

 private:
  void computeIrreducibleDecom(Ideal& ideal,
							   TermConsumer* consumer,
							   bool preMinimized);
  void computeAlexanderDual(BigIdeal& ideal,
							const vector<mpz_class>& point,
							bool useLcm,
							FILE* out,
							const string& format);

  void runSliceAlgorithm(Ideal& ideal, SliceStrategy* strategy);

  const IrreducibleDecomParameters& _parameters;
};

#endif
