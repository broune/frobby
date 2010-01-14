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
class TermTranslator;

#include "Term.h"
#include "Ideal.h"
#include "HashPolynomial.h"
#include "UniHashPolynomial.h"
#include <vector>

/** This class handles the base cases for the Hilbert-Poincare series
 by Bigatti et.al.
*/
class BigattiBaseCase {
 public:
  /** Initialize this object to handle the computation of
      Hilbert-Poincare series numerator polynomials in a polynomial
      ring with varCount variables. */
  BigattiBaseCase(const TermTranslator& translator);

  /** Returns ture if state is a base case slice while also
   considering genericity. This generalizes the functionality of
   baseCase(). */
  bool genericBaseCase(const BigattiState& state);

  /** Returns true if state is a base case slice without considering
   genericity. */
  bool baseCase(const BigattiState& state);

  /** Add +term or -term to the output polynomial when plus is true or
   false respectively. */
  void output(bool plus, const Term& term);

  /** Feed the output Hilbert-Poincare numerator polynomial computed
   so far to the consumer. This is done in canonical order if
   inCanonicalOrder is true. */
  void feedOutputTo(CoefBigTermConsumer& consumer, bool inCanonicalOrder);

  /** Starts to print debug output on what happens if value is
   true. */
  void setPrintDebug(bool value);

  /** Use the fine grading if value is false, otherwise grade each
   variable by the same variable t. */
  void setComputeUnivariate(bool value);

  /** Returns the total number of base cases this object has seen. */
  size_t getTotalBaseCasesEver() const;

  /** Returns the total number of terms this object has output. This
   can be substantially more than the number of terms in the output
   polynomial, since the sum of two terms can be just one term or even
   zero. */
  size_t getTotalTermsOutputEver() const;

  /** Returns the number of terms in the output polynomial right
   now. */
  size_t getTotalTermsInOutput() const;

 private:
  /** Computes the Hilbert-Poincare series of state and returns true
   if state is a particularly simple and easily detected case. */
  bool simpleBaseCase(const BigattiState& state);

  bool univariateAllFaces(const BigattiState& state);

  /** The ideal in state must be weakly generic. Then the
   Hilbert-Poincare series is computed by enumerating the facet of the
   Scarf complex.

   @param allFaces If true then every subset of monomial ideals is a
    facet of the Scarf complex. This allows for faster computation if
    true but yields incorrect results if not.
  */
  void enumerateScarfComplex(const BigattiState& state, bool allFaces);

  vector<size_t> _maxCount;
  Term _lcm;
  mpz_class _tmp;

  /** The part of the finely graded Hilbert-Poincare numerator
   polynomial computed so far. */
  HashPolynomial _outputMultivariate;

  /** The part of the coarsely graded Hilbert-Poincare numerator
   polynomial computed so far. */
  UniHashPolynomial _outputUnivariate;

  /** Used in enumerateScarfComplex and necessary to have here to
   define _states. */
  struct State {
    Term term;
    Ideal::const_iterator pos;
    bool plus;
  };

  /** Used in enumerateScarfCompex. Is not a local variable to avoid
   the cost of re-allocation at every call. */
  vector<State> _states;

  /** Use the fine grading if false, otherwise grade each variable by
   the same variable t. */
  bool _computeUnivariate;

  /** Used to translate the output from ints. */
  const TermTranslator& _translator;

  /** For statistics. Can overflow. */
  size_t _totalBaseCasesEver;

  /** For statistics. Can overflow. */
  size_t _totalTermsOutputEver;

  bool _printDebug;
};

#endif
