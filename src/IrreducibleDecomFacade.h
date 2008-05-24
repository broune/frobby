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
#ifndef IRREDUCIBLE_DECOM_FACADE_GUARD
#define IRREDUCIBLE_DECOM_FACADE_GUARD

#include "Facade.h"
#include <vector>

class IrreducibleDecomParameters;
class BigIdeal;
class Strategy;
class TermTranslator;
class MsmStrategy;
class Ideal;
class TermConsumer;

#include "BigTermConsumer.h"

class IrreducibleDecomFacade : private Facade {
 public:
  IrreducibleDecomFacade(bool printActions,
						 const IrreducibleDecomParameters& parameters);

  void printLabels(BigIdeal& ideal, FILE* out, const string& format);

  // Feed the irreducible decomposition of ideal to consumer and clear
  // ideal.
  void computeIrreducibleDecom(BigIdeal& ideal, BigTermConsumer* consumer);

  // Feed the minimal generators of the Alexander dual of ideal to
  // consumer and clear ideal. point is used as a the parameter, and
  // if it is null, then the least common multiple of the minimal
  // generators of ideal is used.
  //
  // If non-null, point must be divisible by the least common multiple
  // of the generators of ideal.
  void computeAlexanderDual(BigIdeal& ideal,
							const vector<mpz_class>* point,
							BigTermConsumer* consumer);

  void computeFrobeniusNumber(const vector<mpz_class>& instance,
							  BigIdeal& ideal, 
							  mpz_class& frobeniusNumber,
							  vector<mpz_class>& vector);

  void computeIrreducibleDecom(Ideal& ideal, TermConsumer* consumer);

 private:
  void computeIrreducibleDecom(Ideal& ideal,
							   TermConsumer* consumer,
							   bool preMinimized);

  void runSliceAlgorithm(Ideal& ideal, MsmStrategy* strategy);

  const IrreducibleDecomParameters& _parameters;
};

#endif
