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
#ifndef FROBENIUS_STRATEGY_GUARD
#define FROBENIUS_STRATEGY_GUARD

#include "MsmStrategy.h"
#include "Term.h"
#include "TermConsumer.h"

class Slice;
class TermGrader;

// TODO: Rename this class to something more appropriate, such as
// OptimizationStrategy, since it is no onger specific to Frobenius
// number computations.
class FrobeniusStrategy : public MsmStrategy, public TermConsumer {
public:
  FrobeniusStrategy(TermGrader& grader,
					const SplitStrategy* splitStrategy,
					bool reportAllSolutions,
					bool useBound);

  const Ideal& getMaximalSolutions();

  // The optimal value associated to each entry from
  // getMaximalSolutions(). This method can only be called if
  // getMaximalSolutions() is not the zero ideal.
  const mpz_class& getMaximalValue();

private:
  virtual void getPivot(Term& pivot, Slice& slice);
  virtual void simplify(Slice& slice);

  virtual void beginConsuming();
  virtual void consume(const Term& term);
  virtual void doneConsuming();

  Exponent improveLowerBound(size_t var,
							 const mpz_class& upperBoundDegree,
							 const Term& upperBound,
							 const Term& lowerBound);

  void getUpperBound(const Slice& slice, Term& bound);

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
  mpz_class _improveLowerBound_value;
  mpz_class _consume_degree;
  mpz_class _getPivot_maxDiff;
  mpz_class _getPivot_diff;
  mpz_class _simplify_degree;
  mpz_class _improveLowerBound_baseUpperBoundDegree;

  Term _simplify_bound;
  Term _simplify_oldBound;
  Term _simplify_colon;
};

#endif
