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
#ifndef BIGATTI_BASE_CASE_GUARD
#define BIGATTI_BASE_CASE_GUARD

class BigattiState;

#include "Term.h"
#include "Ideal.h"
#include "HashPolynomial.h"
#include <vector>

class BigattiBaseCase {
 public:
  BigattiBaseCase(size_t varCount);

  bool genericBaseCase(const BigattiState& state);
  void generic(const Term& term, Ideal::const_iterator pos, bool plus);

  bool baseCase(const BigattiState& state);
  
  void outputPlus(const Term& term);
  void outputMinus(const Term& term);

  void feedOutputTo(CoefTermConsumer& consumer);

  void printDebug(bool value);

  size_t getTotalBaseCasesEver() const;
  size_t getTotalTermsOutputEver() const;
  size_t getTotalTermsInOutput() const;

 private:
  bool simpleBaseCase();

  void allCombinations();
  bool nextCombination();
  void take(size_t gen);
  void drop(size_t gen);

  size_t _varCount;

  vector<size_t> _maxCount;
  Term _lcm;

  const BigattiState* _state;
  vector<int> _taken;
  Ideal _lcms;
  size_t _takenCount;

  HashPolynomial _output;

  mpz_class _one;
  mpz_class _minusOne;

  /** For statistics. Not a disaster if it overflows. */
  size_t _totalBaseCasesEver;

  /** For statistics. Not a disaster if it overflows. */
  size_t _totalTermsOutputEver;

  /** Print debug messages about what is happening. */
  bool _printDebug;
};

#endif
