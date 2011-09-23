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
  /** The values of BoundSetting indicate how to use the bound. */
  enum BoundSetting {
    /** Make no use of the bound. */
    DoNotUseBound,

    /** Eliminate non-improving slices, achieving a branch-and-bound
     algorithm in place of the usual backtracking. */
    UseBoundToEliminate,

    /** Eliminate non-improving slices and simplify slices by trying
     to generate non-improving slices that are then eliminated,
     allowing to move to the other slice. */
    UseBoundToEliminateAndSimplify
  };

  /** Construct an OptimizeStrategy.

   @param grader This object assigns values to monomials.

   @param splitStrategy The split selection strategy to use.

   @param reportAllSolutions Compute all msm's of optimal value if
   true. Otherwise compute a single msm of optimal value.

   @param boundSetting Indicates how much to use the bound.
  */
  OptimizeStrategy(TermGrader& grader,
                   const SplitStrategy* splitStrategy,
                   bool reportAllSolutions,
                   BoundSetting boundSetting);

  /** Returns one of or all of the msm's with optimal value found so
   far, depending on the value of reportAllSolutions passed to the
   constructor.
  */
  const Ideal& getMaximalSolutions();

  /** The optimal value associated to all entries from
   getMaximalSolutions(). This method can only be called if
   getMaximalSolutions() is not the zero ideal.
  */
  const mpz_class& getMaximalValue();

  /** Independence splits are not supported, so calling this method
   does nothing. Will assert in debug mode if use is true.
  */
  virtual void setUseIndependence(bool use);

  virtual void getPivot(Term& pivot, Slice& slice);

  /** This method calls MsmStrategy::simplify to perform the usual
   simplification of slice, which then occurs if and only if the usual
   simplification has been turned on.

   Independent of whether usual simplification has been turned on,
   this method also eliminates non-improving slices, and uses
   bound-driven simplification, depending on the value of BoundSetting
   passed to the constructor.
  */
  virtual bool simplify(Slice& slice);

  virtual void beginConsuming();
  virtual void consume(const Term& term);
  virtual void doneConsuming();

 private:
  /** Returns true if iterating bound-based simplification might do
   something. There are four cases where it makes sense to iterate the
   bound-based simplification after the slice has changed.

    - Case 1: The sign is negative, and the divisor has increased. This is
      relevant whether or not we are using a fake power, since if so
      there still is an effect on the bound for a split that gets rid
      of the pure power.

    - Case 2: The sign is negative, and we were using a fake power to get the
      value of zero, but now it is gone.

    - Case 3: The sign is positive, and the dominator has decreased,
      and it did not just remove a fake power exponent.

    - Case 4: The sign is positive, and the dominator has a fake power
      exponent, and the divisor has increased so that we are forced to
      use it.
  */
  bool changedInWayRelevantToBound
    (const Term& oldDivisor, const Term& oldDominator,
     const Term& newDivisor, const Term& newDominator) const;

  /** This method simplifies a slice based on generating non-improving
   outer and inner slices. The idea is conceptually simple, but it has
   been quite a challenge to get the code to be correct, efficient and
   as good as possible in terms of simplifying as much as
   possible. This is why there are so many tests related to this, and
   why the comments for this and related methods are so detailed.

   To each slice we associate a divisor \f$x^a\f$ and a dominator
   \f$x^b\f$, which respectively divide and dominate each msm \f$m\f$
   in the content of the slice. I.e. \f$x^a|m|x^b\f$. We summarize
   this as a pair \f$(a,b)\f$. Let \f$d(a,b)\f$ be the associated
   upper bound, i.e. the largest possible value that the grader
   associates to a monomial \f$x^v\f$ such that \f$a\leq v\leq
   b\f$. Thus \f$d(a,b)\f$ is an uppper bound on the value of any msm
   in the content of the slice to which \f$(a,b)\f$ is associated.

   To be more exact, for a slice \f$(I,S,q)\f$, \f$q\f$ is the
   divisor, while \f$q\textrm{lcm}(\min(I)):x_1\cdots x_n\f$ is the
   dominator. Also, let \f$d_i(t)\f$ be the value of \f$x_i^t\f$ and
   let \f$\max\f$ be the highest value according to the grader of an
   msm found so far.

   This allows us to discard slices that are non-improving, in that
   they do not improve on the best value found so far. Note that
   whether "improve" means "strictly improve" or "weakly improve"
   depends on whether we are reporting all optimal msm's or just
   one. In the following discussion we will assume that we are looking
   for just one msm, so that a slice is non-improving if it does not
   strictly improve.

   This method is called when the slice itself cannot be seen to be
   non-improving from the divisor and dominator. However, if we
   consider pivot splits on pivots that are pure powers, i.e. of the
   form \f$x_i^t\f$, then we can compute a divisor and dominator for
   both the inner and outer slice just from the divisor and dominator
   of the current slice. Note that the divisor and dominator assigned
   in this way are not necessarily as tight as the ones we would get
   if we actually computed the inner and outer slice.

   It may be that we can see that the inner or outer slice from above
   is non-improving. If the inner slice is non-improving, then we can
   discard it and replace the current slice with the outer slice, and
   vice versa if the outer slice is non-improving. This is
   bound-driven simplification of a slice, and it is efficient since
   we can perform these calculations looking only at the two monomials
   divisor and dominator.

   Consider a pivot split on \f$x_i^t\f$. Then the outer slice has
   \f$(a, b^\prime)\f$, where \f$b^\prime\f$ is equal to \f$b\f$
   except that \f$b_i^\prime:= a_i+t-1\f$. The inner slice will have
   \f$(a',b)\f$, where \f$a^\prime\f$ is equal to \f$a\f$ except that
   \f$a^\prime_i := a_i + t\f$. Note that the sensible \f$t\f$'s to
   consider are those for which \f$a_i<a_i+t\leq b_i\f$. We need to
   work in terms of \f$a_i+t\f$ rather than just \f$t\f$ since we work
   with a TermGrader that has no reasonable way of making sense of
   just t.

   If possible, this method moves to an inner or outer slice of the
   parameter slice based on this reasoning.

   See getInnerSimplify and getOuterSimplify for more details. Which
   one of those two is relevant for a given slice and variable depends
   on the sign of that variable in the grading, and on whether the
   ideal contains a pure power that maps to the same value as zero
   does. We call the latter phenomenon a "fake power".

   @param slice The slice to work on.

   @param dominator The dominator of slice (passed as a parameter to
    avoid recomputation).

   @param upperBound The upper bound \f$d(a,b)\f$ associated to slice (to
    avoid recomputation).

   @return Returns true if and only if slice changed.
  */
  bool boundSimplify
    (Slice& slice,
     const Term& dominator,
     const mpz_class& upperBound);

  /** Find an outer slice that is non-improving, allowing us to
   replace the current slice with the inner slice. We only need to
   know a divisor \f$a\f$ and dominator \f$b\f$ of the current slice to
   perform this analysis. If \f$a_i=b_i\f$ then there is nothing to be
   done for the \f$i\f$'th entry, so assume that \f$a_i<b_i\f$.

   There are two ways to obtain a non-improving outer slice. The first
   is if the grading sign is positive, and \f$d(a,b^\prime)\leq
   \max\f$. Let \f$B:=b_i\f$ if \f$b_i\f$ is not a fake exponent, and let
   \f$B := b_i-1\f$ if \f$b_i\f$ is a fake exponent. Then we have that \f[
     d(a,b^\prime) = d(a,b) - d_i(B) + d_i(a_i+t-1),
   \f] whereby \f$d(a,b^\prime)\leq \max\f$ is equivalent to \f[
     d_i(a_i+t-1) \leq \max - d(a,b) + d_i(B) =: C.
   \f]

   Letting \f$t^\prime:=a_i+t-1\f$, we are thus looking for a
   \f$t^\prime\f$ with \f$a_i\leq t^\prime<B\f$ such that
   \f$d_i(t^\prime)\leq C\f$. To make the inner slice as much simpler
   as possible, we would like to find the largest such t, if
   any. TermGrader has a method that does this.

   The other way of getting a non-improving outer slice is with a
   negative grading sign and a fake power. The fake power allows to
   set the value at the i'th entry down to the value of zero, which is
   better than any other exponent when the sign is negative.

   A non-trivial outer slice will not have the fake power, and thus
   possibly decrease the upper bound of the outer slice relative to
   the current slice, which may make it non-improving. This is most
   likely to happen for the largest possible \f$t=b_i\f$, and we want
   \f$t\f$ to be large anyway in order to make the inner slice as
   simple as possible, so it only makes sense to check this for
   \f$t=b_i-a_i\f$. As the sign is negative, we get that \f[
     d(a,b^\prime)=d(a,b)-d_i(b_i)+d_i(a_i)=:C,
   \f] where we are noting that \f$d_i(b_i)=d_i(0)\f$. The question is then
   whether \f$C\leq\max\f$.

   @param divisor The divisor \f$a\f$ associated to the slice.

   @param dominator The dominator \f$b\f$ associated to the slice.

   @param upperBound The upper bound \f$d(a,b)\f$ associated to the slice
   (passed as a parameter to avoid recomputation).

   @param pivot Is set to a pivot that generates a non-improving
    outer slice if one is found.

   @return Returns true if and only if a non-improving outer slice was
    found.
  */
  bool getInnerSimplify
    (const Term& divisor,
     const Term& dominator,
     const mpz_class& upperBound,
     Term& pivot);

  /** Find an inner slice that is non-improving, allowing us to
   replace the current slice with the outer slice. We only need to know a
   divisor \f$a\f$ and dominator \f$b\f$ of the current slice to perform this
   analysis. If \f$a_i=b_i\f$ then there is nothing to be done for the
   \f$i\f$'th entry, so assume that \f$a_i<b_i\f$.

   There are two ways to obtain a non-improving inner slice. The first
   is if the grading sign is negative, there is no fake power of
   \f$x_i\f$, and \f$d(a^\prime,b)\leq \max\f$. Since the sign is
   negative and there is no fake power, we have that \f[
     d(a^\prime,b) = d(a,b) - d_i(a_i) + d_i(a_i+t),
   \f], whereby \f$d(a^\prime,b)\leq \max\f$ is equivalent to \f[
     d_i(a_i+t) \leq \max - d(a,b) + d_i(a_i) =: C.
   \f]

   Letting \f$t^\prime:=a_i+t\f$, we are thus looking for a
   \f$t^\prime\f$ with \f$a_i< t^\prime\leq b_i^\prime\f$ such that
   \f$d_i(t^\prime)\leq C\f$. To make the outer slice as much simpler
   as possible, we would like to find the smallest such t, if
   any. TermGrader has a method that does this.

   Note that the assumption above that there is no fake power of
   \f$x_i\f$ does not rule out any useful simplification, as in that
   case the inner slice will never be non-improving, since it would
   also have the fake power (in our analysis of just looking at
   \f$a\f$ and \f$b\f$), so that the upper bound of the inner slice
   would be equal to the upper bound of the current slice.

   The other way of getting a non-improving inner slice is with a
   positive grading sign and a fake power. If \f$t\f$ is as large as
   possible, i.e. \f$t=b_i-a_i\f$, then the outer slice is forced to
   use the fake power, which decreases the \f$i\f$'th entry down to
   the value for zero, which is the worst possible when the sign is
   positive, and this might make the inner slice non-improving.

   As the sign is positive, and \f$b_i\f$ is a fake exponent, we get
   that \f[
     d(a^\prime,b)=d(a,b)-d_i(b_i-1)+d_i(b_i):=C,
   \f] and it is then a question of whether \f$C\leq\max\f$.

   @param divisor The divisor \f$a\f$ associated to the slice.

   @param dominator The dominator \f$b\f$ associated to the slice.

   @param upperBound The upper bound \f$d(a,b)\f$ associated to the slice
    (passed as a parameter to avoid recomputation).

   @param pivot Is set to a pivot that generates a non-improving
    inner slice if one is found.

   @return Returns true if and only if a non-improving inner slice was
    found.
  */
  bool getOuterSimplify
    (const Term& divisor,
     const Term& dominator,
     const mpz_class& upperBound,
     Term& pivot);

  /** Sets dominator to be a term dominating every element of the
   content of slice.

   Returns false (and clears slice) only if slice is a trivial base
   case slice, but may return true even if it is a base case slice.
  */
  bool getDominator(Slice& slice, Term& dominator);

  /** The number of varibles this object was initialized with. */
  size_t getVarCount() const;

  /** We use _grader to assign values to solutions. */
  const TermGrader& _grader;

  /** The best value of any solution found so far. The value is
   undefined if no solution has been found so far.
  */
  mpz_class _maxValue;

  /** Is equal to _maxValue minus _reportAllSolutions, except when no
   solution has been found so far, in which case the value is
   undefined. We use _maxValueToBeat in place of _maxValue as a way to
   handle both values of _reportAllSolutions without having a lot of
   code of the type

    if (_reportAllSolutions ? a < _maxValue : a <= _maxValue) ...

   The point is that we can replace this by

     if (a <= _maxValueToBeat) ...
  */
  mpz_class _maxValueToBeat;

  /** Stores the optimal solutions found so far, according to the best
   value found so far.
  */
  Ideal _maxSolutions;

  /** Indicates whether to compute all optimal solutions, as opposed
   to computing just one (when there are any).
  */
  bool _reportAllSolutions;

  /** Indicates how to use the bound. */
  BoundSetting _boundSetting;

  /** Temporary variable used in consume. Is a member variable in
   order to avoid the cost of initializing an mpz_class every time.
  */
  mpz_class _consume_tmpDegree;

  /** Temporary variable used in simplify. Is a member variable in
   order to avoid the cost of initializing an mpz_class every time.
  */
  mpz_class _simplify_tmpUpperBound;

  /** Temporary variable used in simplify. Is a member variable in
   order to avoid the cost of initializing a Term every time.
  */
  Term _simplify_tmpDominator;

  /** Temporary variable used in simplify. Is a member variable in
   order to avoid the cost of initializing a Term every time.
  */
  Term _simplify_tmpOldDominator;

  /** Temporary variable used in simplify. Is a member variable in
   order to avoid the cost of initializing a Term every time.
  */
  Term _simplify_tmpOldDivisor;

  /** Temporary variable used in getInnerSimplify and
   getOuterSimplify. Is a member variable in order to avoid the cost
   of initializing an mpz_class every time.
  */
  mpz_class _tmpC;

  /** Temporary variable used in simplify. Is a member variable in
   order to avoid the cost of initializing a Term every time.
  */
  Term _boundSimplify_tmpPivot;


  FRIEND_TEST(OptimizeStrategy, ChangedInWayRelevantToBound);
  FRIEND_TEST(OptimizeStrategy, SimplifyPositiveGrading);
  FRIEND_TEST(OptimizeStrategy, SimplifyNegativeGrading);
};

#endif
