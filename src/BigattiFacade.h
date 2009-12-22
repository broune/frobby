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
#ifndef BIGATTI_FACADE_GUARD
#define BIGATTI_FACADE_GUARD

#include "Facade.h"
#include "BigattiPivotStrategy.h"
#include "TermTranslator.h"
#include "Ideal.h"

class BigIdeal;
class IOHandler;
class CoefTermConsumer;

/** A facade for computing Hilbert series of monomial ideals using the
 divide-and-conquer algorithm by Bigatti et.al.

 @ingroup Facade
*/
class BigattiFacade : public Facade {
 public:
  /** The constructed object can run the Bigatti algorithm on ideal.

   @param ideal The ideal to run algorithms on.
   @param handler Defines the format in which to produce output.
   @param out An open file to write output to.
   @param printActions Display subtasks and CPU time spent.
  */
  BigattiFacade(const BigIdeal& ideal,
				IOHandler* handler,
				FILE* out,
				bool printActions = false);

  /** Sort the variable in a canonical order, permuting the entries of
   generators of _ideal to match. */
  void sortVars();

  /** Print statistics on what the algorithm did to standard error if
	  value is true. */
  void setPrintStatistics(bool value);

  /** Print information about what the algorithm is doing to standard
	  error if value is true. */
  void setPrintDebug(bool value);

  /** Detect generic monomial ideals as a base case for the algorithm
	  if value is true. */
  void setUseGenericBaseCase(bool value);

  /** Set the pivot selection strategy for pivots splits used by the
	  algorithm. */
  void setPivotStrategy(auto_ptr<BigattiPivotStrategy> pivot);

  /** Specify whether the input ideal is known to be minimally
   generated. An ideal is minimally generated if no generator divides
   any other generator. The default is to assume that the input ideal
   is not known to be minimally generated.

   If the ideal is minimally generated, setting this option can
   improve performance. However, if this option is set and the ideal
   is not actually minimally generated, then Frobby will exhibit
   undefined behavior. It might crash, it might go into an infinite
   loop or it might produce incorrect results.
  */
  void setIsMinimallyGenerated(bool value);

  /** Produce output in a canonical order. This involves sorting the
   variables as well as the individual terms.
  */
  void setDoCanonicalOutput(bool value);

  /** Apply simplification of a BigattiState when possible. */
  void setUseSimplification(bool value);

  /** Compute the numerator of the multigraded Hilbert-Poincare
   series.

   This series is the sum of all monomials not in the ideal.

   The multigraded Hilbert-Poincare series can be written as a single
   fraction with a polynomial as the numerator and the product of
   (1-x) as the denominator where x varies over all the variable in
   the ambient ring. It is this numerator that is computed.
  */
  void computeMultigradedHilbertSeries();

  /** Compute the numerator of the univariate Hilbert-Poincare
   series.

   This is defined as the multivariate Hilbert-Poincare series where
   every variable has been substituted with the same single variable
   t. See the documentation for computeMultigradedHilbertSeries() for
   more information.

   The terms of the output polynomial are provided in ascending order
   according to exponent.
  */
  void computeUnivariateHilbertSeries();

 private:
  /** Run the algorithm.

   @param action Action to display (if displaying actions).
   @param consumer Pass the output to this consumer.
  */
  void runAlgorithm(const char* action, CoefTermConsumer& consumer);

  /** Removes any non-minimal generators of the input ideal.

   This does nothing if the ideal has been specified to be already
   minimally generated, which can be done by calling
   setIsMinimallyGenerated(true). This is done by this method itself,
   so calling it many times performs only one minimization.
  */
  void minimize();

  /** Produce output in a canonical representation. */
  bool _canonicalOutput;

  /** The ideal is minimally generated. */
  bool _isMinimallyGenerated;

  /** Print statistics on what the algorithm did. */
  bool _printStatistics;

  /** Print debug output as the algorithm runs. */
  bool _printDebug;

  /** Detect generic ideals as a base case. */
  bool _useGenericBaseCase;

  /** Apply simplification of BigattiState when possible. */
  bool _useSimplification;

  /** Use this pivot selection strategy for pivot splits. */
  auto_ptr<BigattiPivotStrategy> _pivot;

  /** The ideal to run the algorithm on. */
  Ideal _ideal;

  /** Produce output in a canonical representation. */
  bool _doCanonicalOutput;

  /** Translator for _ideal. */
  auto_ptr<TermTranslator> _translator;

  /** The format in which to produce output. */
  IOHandler* _ioHandler;

  /** The file to write output to. */
  FILE* _out;
};

#endif
