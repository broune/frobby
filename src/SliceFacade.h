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
#include "Facade.h"
#include <vector>
#include <cstdio>
#include <string>

class BigIdeal;
class BigTermConsumer;
class CoefBigTermConsumer;
class TermTranslator;
class CoefTermConsumer;
class Ideal;
class SliceStrategy;
class TermConsumer;
class IOHandler;

// This class acts as a simple interface to the slice-like
// algorithms. The intended (but not required) use is to construct a
// facade, perform one computation and then destruct the facade. This
// is an instance of the Facade design pattern.
class SliceFacade : public Facade {
 public:
  // The constructors take an ideal, a consumer and a boolean
  // printActions. The constructed facade will run algorithms on the
  // ideal, provided that the output this provides is the kind of
  // output accepted by the consumer. If printActions is true, then
  // print the amount of CPU time taken by the algorithm to stderr.
  //
  // The facade copies ideal and does not maintain a reference to any
  // part of it. The consumer is not deleted when the facade is
  // destructed.

  // Enable algorithms that produce monomials as output.
  SliceFacade(const BigIdeal& ideal,
			  BigTermConsumer* consumer,
			  bool printActions = false);

  // Enable algorithms that produce a polynomial as output.
  SliceFacade(const BigIdeal& ideal,
			  CoefBigTermConsumer* consumer,
			  bool printActions = false);

  SliceFacade(const BigIdeal& ideal,
			  IOHandler* handler,
			  FILE* out,
			  bool printActions = false);

  ~SliceFacade();

  // Print debug output to stderr as the algorithm runs. The default
  // is to not do so.
  void setPrintDebug(bool printDebug);

  // Print statistics related to the run of the algorithm. The default
  // is to not do so.
  void setPrintStatistics(bool printStatistics);

  // Specify whether to use independence splits. The default is to use
  // them.
  void setUseIndependence(bool useIndependence);

  // Specify whether the input ideal is known to be minimally
  // generated. This means that no generator divides any other
  // generator. Setting this to true can increase performance, but
  // this leads to unpredictable behavior, including the risk of an
  // infinite loop, if the ideal is not actually minimally
  // generated. The default is to assume that the input ideal is not
  // necessarily minimally generated.
  void setIsMinimallyGenerated(bool isMinimallyGenerated);

  // If strategyName is the name of a valid split selection strategy,
  // then use that strategy and return true. Otherwise return false.
  // allowLabelSplits specifies whether strategies for label splits
  // are valid. It is an error to run an algorithm that does not
  // support label splits while using a label split selection
  // strategy.
  bool setSplitStrategy(const string& strategyName,
						bool allowLabelSplits = false,
						bool allowFrobeniusSplit = false);

  // Compute the numerator of the multigraded Hilbert-Poincare series
  // expessed as a rational function. If canonical is true, then the
  // terms of the output polynomial are provided in lexicographical
  // order according to exponent vectors. Otherwise the terms are
  // provided in arbitrary order.
  void computeMultigradedHilbertSeries(bool canonical);

  // Compute the numerator of the univariate (i.e. the grading is the
  // total degree) Hilbert-Poincare series expessed as a rational
  // function. The terms of the output polynomial are provided in
  // ascending order according to exponent.
  void computeUnivariateHilbertSeries();

  // Compute the unique irredundant set of irreducible ideals whose
  // intersection equals ideal. If encode is false, then each
  // irreducible component is output as a separate ideal, which is
  // only supported when writing to a file. If encode is true, then
  // each irreducible component is provided as a term, where missing
  // pure powers are encoded as a zero exponent. The irreducible
  // components are provided in arbitrary order.
  void computeIrreducibleDecomposition(bool encode);

  // Compute each maximal staircase monomial. A monomial m is a
  // staircase monomial if it belongs to the ideal, while the monomial
  // m : (x_1 ... x_n) does not. A monomial m is a maximal staircase
  // monomial if m is a staircase monomial and m * x_i is not for
  // every variable x_i. The output monomials are provided in
  // arbitrary order.
  void computeMaximalStaircaseMonomials();

  // Compute each maximal standard monomial. A monomial m is a
  // standard monomial if it does not belong to the ideal. A monomial
  // m is a maximal standard monomial if it is standard and m * x_i is
  // not standard for every variable x_i. The output monomials are
  // provided in arbitrary order.
  void computeMaximalStandardMonomials();

  // Compute the minimal generators of the Alexander dual of the
  // ideal, using point as the point parameter of the Alexander
  // dual. It is an error if the lcm of the minimal generators of
  // ideal does not divide the point. The output monomials are
  // provided in arbitrary order.
  //
  // It is an error for the size of point to be different from the
  // number of variables of ideal.
  void computeAlexanderDual(const vector<mpz_class>& point);

  // Like computeAlexanderDual, except that the point is set to the
  // lcm of the minimal geneators of ideal.
  void computeAlexanderDual();

  // Compute the associated primes of ideal and represent them by
  // square free terms. The terms are provided in arbitrary order.
  void computeAssociatedPrimes();

  // Output an optimal solution to the following optimization problem.
  //   
  //   maximize v * grading such that x ^ v is a maximal standard
  //   monomial of ideal
  //
  // Produces no output if ideal has no maximal standard monomials.
  //
  // The parameter useBound specifies whether or not to use
  // brand-and-bound to speed the computation up.
  void solveStandardProgram(const vector<mpz_class>& grading,
							bool useBound);

 private:
  // Common code from the constructors.
  void initialize(const BigIdeal& ideal);

  // Remove non-minimal generators unless the ideal has been specified
  // to be already minimally generated.
  void minimize();

  // Puts the least common multiple of ideal into lcm.
  void getLcmOfIdeal(vector<mpz_class>& lcm);

  void runSliceAlgorithmAndDeleteStrategy(SliceStrategy* strategy);

  // Output each term as an irreducible ideal. Cannot be called after
  // getTermConsumer has been called. Can only be called when writing
  // terms without coefficient to a file.
  void doIrreducibleIdealOutput();

  TermConsumer* getTermConsumer();
  CoefTermConsumer* getCoefTermConsumer(bool canonical);

  bool _printDebug;
  bool _printStatistics;
  bool _useIndependence;
  bool _isMinimallyGenerated;

  FILE* _out;
  IOHandler* _ioHandler;

  BigTermConsumer* _termConsumer;
  CoefBigTermConsumer* _coefTermConsumer;

  TermConsumer* _generatedTermConsumer;
  CoefTermConsumer* _generatedCoefTermConsumer;

  SplitStrategy _split;

  // These are points to avoid including more headers than necessary.
  TermTranslator* _translator;
  Ideal* _ideal;
};

#endif
