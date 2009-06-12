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
#ifndef OPTIMIZE_STRATEGY_GUARD
#define OPTIMIZE_STRATEGY_GUARD

#include "MsmStrategy.h"
#include "Term.h"
#include "TermConsumer.h"
#include "tests.h"

class Slice;
class TermGrader;

/** OptimizeStrategy optimizes a function on the maximal standard
 monomials of a monomial ideal using branch-and-bound.

 Branch-and-bound is an algorithmic technique. In this case, it
 amounts to having a bound, which is a function that assigns a value
 to each slice. This value is an non-strict upper bound on the value
 of each maximal standard monomial in the content of that slice. If
 the value of the bound is worse than the best value of an msm found
 so far, then we do not have to consider that slice further. Such a
 slice is called non-improving.

 Whenever a slice is non-improving, we avoid some computation by
 ignoring it. We exploit this further, by purposefully seeking to
 generate slices that are non-improving.
*/
class OptimizeStrategy : public MsmStrategy, public TermConsumer {
public:
  OptimizeStrategy(TermGrader& grader,
				   const SplitStrategy* splitStrategy,
				   bool reportAllSolutions,
				   bool useBound);

  const Ideal& getMaximalSolutions();

  /** The optimal value associated to each entry from
   getMaximalSolutions(). This method can only be called if
   getMaximalSolutions() is not the zero ideal.
  */
  const mpz_class& getMaximalValue();

  /** Independence splits are not supported, so calling this method
   does nothing. Will assert in debug mode if use is true.
  */
  virtual void setUseIndependence(bool use);

  virtual void getPivot(Term& pivot, Slice& slice);
  virtual void simplify(Slice& slice);

  virtual void beginConsuming();
  virtual void consume(const Term& term);
  virtual void doneConsuming();

 private:
  /** We are considering a slice where each msm d in the content
    fulfills that lowerBound divides d and d divides
    upperBound. upperBoundDegree is the degree of upperBound and is
    passed as a parameter so that it does not have to be recomputed.

	Suppose we perform a pivot split on var^e for some e. Then the
	outer slice will have a monomial upper bound whose exponent of var
	is at most e. So if the degree of upperBound with the exponent of
	var replaced by e is worse than the best value found so far, then
	this outer slice will be non-promising. Thus we only have to
	consider the inner slice, which will have lowerBound increased by
	var^e. This method returns the largest value of e for which this
	works.

	TODO: update.
   */

  bool boundSimplify
	(Slice& slice,
	 const Term& dominator,
	 const mpz_class& degree);

  bool getInnerSimplify
	(const Term& divisor,
	 const Term& dominator,
	 const mpz_class& degree,
	 Term& pivot);
  bool getOuterSimplify
	(const Term& divisor,
	 const Term& dominator,
	 const mpz_class& degree,
	 Term& pivot);


  /** Sets dominator to be a term dominating every element of the
   content of slice.

   Returns false (and clears slice) only if slice is a base case
   slice, but may return true even if it is a base case slice.
  */
  bool getDominator(Slice& slice, Term& dominator);

  size_t getVarCount() const;

  const TermGrader& _grader;

  mpz_class _maxValue;
  Ideal _maxSolutions;

  bool _reportAllSolutions;
  bool _useBound;

  // These are in place of using static variables. The static
  // variables were introduced because constructing these objects took
  // around 10% of the running time, and sticking static in front of
  // the variable names was a very easy way to improving speed by
  // 10%. They changed into this form because a compiler bug on OS X
  // gcc made it impossible to do static mpz_class's, and because the
  // memory contained in these really should be freed when the
  // strategy is deleted.
  //
  // The format is _methodOfUse_nameOfVariable. Several of these could
  // be merged into one variable, but this could easily introduce a
  // bug through an unexpected alias and the cost of keeping them
  // seperate is negligible.
  mpz_class _improveLowerBound_maxExponent;
  mpz_class _consume_degree;
  mpz_class _simplify_degree;

  Term _simplify_dominator;
  Term _simplify_oldDominator;

  Term _improvement;

  FRIEND_TEST(OptimizeStrategy, improveBoundExponent);
  FRIEND_TEST(OptimizeStrategy, simplifyPositiveGrading);
  FRIEND_TEST(OptimizeStrategy, simplifyNegativeGrading);
};

#endif
