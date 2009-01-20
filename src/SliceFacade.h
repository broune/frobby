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
#ifndef SLICE_FACADE_GUARD
#define SLICE_FACADE_GUARD

#include "SplitStrategy.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include "Facade.h"
#include "TermConsumer.h"
#include "CoefTermConsumer.h"

#include <vector>
#include <cstdio>
#include <string>

class BigIdeal;
class BigTermConsumer;
class CoefBigTermConsumer;
class TermTranslator;
class SliceStrategy;
class IOHandler;

/** This class acts as a simpler interface to the slice-like
 algorithms. This class is an instance of the <a
 href="http://en.wikipedia.org/wiki/Facade_pattern">Facade design
 pattern</a>.
*/
class SliceFacade : public Facade {
 public:
  /** The constructed object can run an algorithm on the ideal if it
   produces monomial ideal output.

   @param ideal The ideal to run algorithms on.
   @param consumer Monomial ideal output will be fed to this consumer.
   @param printActions Display subtasks and CPU time spent.
  */
  SliceFacade(const BigIdeal& ideal,
			  BigTermConsumer* consumer,
			  bool printActions = false);

  /** The constructed object can run an algorithm on the ideal if it
   produces polynomial output.

   @param ideal The ideal to run algorithms on.
   @param consumer Polynomial output will be fed to this consumer.
   @param printActions Display subtasks and CPU time spent.
  */
  SliceFacade(const BigIdeal& ideal,
			  CoefBigTermConsumer* consumer,
			  bool printActions = false);

  /** The constructed object can run any algorithm on the ideal.

   @param ideal The ideal to run algorithms on.
   @param handler Defines the format in which to produce output.
   @param out An open file to write output to.
   @param printActions Display subtasks and CPU time spent.
  */
  SliceFacade(const BigIdeal& ideal,
			  IOHandler* handler,
			  FILE* out,
			  bool printActions = false);

  /** Print debug output to standard error as the algorithm runs. The
   default is to not do so. */
  void setPrintDebug(bool printDebug);

  /** Print statistics related to the run of the algorithm. The
   default is to not do so. */
  void setPrintStatistics(bool printStatistics);

  /** Specify whether to use independence splits. The default is to
   use them. */
  void setUseIndependence(bool useIndependence);

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
  void setIsMinimallyGenerated(bool isMinimallyGenerated);

  /** Set the split selection strategy used by the Slice Algorithm. It
   is an error to run an algorithm that does not support the specified
   split strategy.
  */
  void setSplitStrategy(auto_ptr<SplitStrategy> split);

  /** Produce output in a canonical order. This involves sorting the
   variables as well as the individual items being produced.
  */
  void setCanonicalOutput();

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

  /** Compute the unique irredundant set of irreducible ideals whose
   intersection equals ideal.

   If encode is false, then each irreducible component is output as a
   separate ideal, which is only supported when writing to a file. If
   encode is true, then each irreducible component is provided as a
   term, where missing pure powers are encoded as a zero exponent.

   The irreducible components are provided in arbitrary order.

   @param encode Specifies whether to encode the irreducible component
   as monomials.

   @todo Check whether encode being false is really only supported for
   writing to a file. If so, fix it and update this documention.
  */
  void computeIrreducibleDecomposition(bool encode);

  /** Compute the unique "nicest" primary decomposition of the
   ideal.

   This is defined as the primary decomposition where each
   primary component is the intersection of the irreducible components
   with that same support.

   Each primary component is provided as a separate ideal in arbitrary
   order.
  */
  void computePrimaryDecomposition();

  /** Compute the maximal staircase monomials of the ideal.
   A monomial m is a staircase monomial if it belongs to the ideal,
   while the monomial m : (x_1 ... x_n) does not. A monomial m is a
   maximal staircase monomial if m is a staircase monomial and m * x_i
   is not for every variable x_i.

   The output monomials are provided in arbitrary order.
  */
  void computeMaximalStaircaseMonomials();

  /** Compute the maximal standard monomials of the ideal.
   A monomial m is a standard monomial if it does not belong to the
   ideal. A monomial m is a maximal standard monomial if it is
   standard and m * x_i is not standard for every variable x_i. The
   output monomials are provided in arbitrary order.
  */
  void computeMaximalStandardMonomials();

  /** Compute the Alexander dual of the ideal.
   The output monomials are provided in arbitrary order.

   It is an error if any minimal generator of ideal does not divide
   point. It is acceptable for a non-minimal generator to not divide
   point.

   @param point The point to dualize on.
  */
  void computeAlexanderDual(const vector<mpz_class>& point);

  /** Compute the Alexander dual of the ideal.
   The point to dualize on is the least common multiple of the minimal
   generators of the ideal.

   The output monomials are provided in arbitrary order.
  */
  void computeAlexanderDual();

  /** Compute the associated primes of the ideal. These are
   represented as generators of an ideal and are provided in arbitrary
   order.

   @todo Add a bool encode option to choose between output as actual
   ideals or as it is being done now.
  */
  void computeAssociatedPrimes();

  /** Solve an optimization program over maximal standard monomials.
   The optimization program being solved is

    maximize \f$\textrm{v} * \textrm{grading}\f$ subject to \f$x ^
    \textrm{v}\f$ being a maximal standard monomial of the ideal.

   @param grading The vector to optimize.
   @param value Will be set to the value of the program, if any.
   @param reportAllSolutions Output all optimal solutions if true,
     otherwise report some optimal solution if there are any.
   @param useBound Specifies whether or not to use branch-and-bound to
   speed the computation up. Note that this is currently only allowed
   if grading has no strictly negative entries.

   @todo Make useBound work for non-positive gradings.
  */
  bool solveStandardProgram
	(const vector<mpz_class>& grading,
	 mpz_class& value,
	 bool reportAllSolutions,
	 bool useBound);

  /** Solve an optimization program over irreducible components.
   The optimization program being solved is

    maximize \f$\textrm{v} * \textrm{grading}\f$ subject to \p v
    encoding an irreducible component of the ideal.

   @param grading The vector to optimize.
   @param optimalValue Will be set to the value of the program, if any.
   @param reportAllSolutions Output all optimal solutions if true,
     otherwise report some optimal solution if there are any.
   @param useBound Specifies whether or not to use branch-and-bound to
   speed the computation up. Note that this is currently only allowed
   if grading has no strictly negative entries.

   @todo Make useBound work for non-positive gradings.
  */
  bool solveIrreducibleDecompositionProgram
	(const vector<mpz_class>& grading,
	 mpz_class& optimalValue,
	 bool reportAllSolutions,
	 bool useBound);

 private:
  /// Common code from solveStandardProgram and
  /// solveIrreducibleDecompositionProgram.
  bool solveProgram
	(const vector<mpz_class>& grading,
	 mpz_class& optimalValue,
	 bool reportAllSolutions,
	 bool useBound);

  /// Common code from the constructors.
  void initialize(const BigIdeal& ideal);

  /** Remove non-minimal generators.

   This does nothing if the ideal has been specified to be already
   minimally generated. This can be done by calling
   setIsMinimallyGenerated(true). Thus calling this method more than
   once is not inefficient, since it records that the ideal is now
   minimally generated.
  */
  void minimize();

  /** Compute the least common multiple of the ideal.
   @param lcm The least common multiple is put here.
  */
  void getLcmOfIdeal(vector<mpz_class>& lcm);

  /// Run the Slice algorithm according to the options set.
  void runSliceAlgorithmWithOptions(SliceStrategy& strategy);

  /** Output each term as the corresponding irreducible ideal.
   Cannot be called after getTermConsumer has been called. Can only be
   called when writing terms without coefficient to a file.

   @todo Is it true that this only works for file output? If so, fix it.
  */
  void doIrreducibleIdealOutput();

  /// Returns the consumer for monomial ideal output.
  TermConsumer* getTermConsumer();

  /// Returns the consumer for polynomial output.
  CoefTermConsumer* getCoefTermConsumer();

  /// Print debug information on the Slice Algorithm.
  bool _printDebug;

  /// Print statistics on the Slice Algorithm.  
  bool _printStatistics;

  /// Use independence splits in the Slice Algorithm.
  bool _useIndependence;

  /// The ideal is minimally generated.
  bool _isMinimallyGenerated;

  /// Produce a canonical representation of the output.
  bool _canonicalOutput;

  /// The file to write output to, if any. 
  FILE* _out;

  /// The format in which to write output to the file, if any.
  IOHandler* _ioHandler;

  /// The passed-in consumer for monomial ideal output.
  BigTermConsumer* _termConsumer;

  /// The passed-in consumer for polynomial output.
  CoefBigTermConsumer* _coefTermConsumer;

  /// Cache for getTermConsumer().
  auto_ptr<TermConsumer> _generatedTermConsumer;

  /// Cache for getCoefTermConsumer().
  auto_ptr<CoefTermConsumer> _generatedCoefTermConsumer;

  /// The split selection strategy to use in the Slice Algorithm.
  auto_ptr<SplitStrategy> _split;

  /// The translator to apply to \p int exponents to get the external
  /// big integers those \p int exponents correspond to.
  auto_ptr<TermTranslator> _translator;

  /// The passed-in ideal after translation to \p int exponents.
  auto_ptr<Ideal> _ideal;
};

#endif
