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
#ifndef TERM_GRADER_GUARD
#define TERM_GRADER_GUARD

#include <vector>

class Projection;
class Term;
class TermTranslator;

/** A TermGrader assigns a value, the degree, to each monomial. */
class TermGrader {
public:
  TermGrader(const vector<mpz_class>& varDegrees,
             const TermTranslator& translator);

  /** Returns the degree of term. */
  mpz_class getDegree(const Term& term) const;

  /** Assigns the degree of term to degree. */
  void getDegree(const Term& term, mpz_class& degree) const;

  /** Assigns the degree of the preimage of term through the
   projection to degree.
  */
  void getDegree(const Term& term,
                 const Projection& projection,
                 mpz_class& degree) const;

  /** Assigns to bound the degree of the largest term v such that
   divisor divides v and v divides dominator.
  */
  void getUpperBound(const Term& divisor,
                     const Term& dominator,
                     mpz_class& bound) const;

  /** Returns the degree of the largest term v such that
   divisor divides v and v divides dominator.

   Returning the value implies a copy so that this overload is less
   efficient than the one placing the value in a parameter. This
   purpose of this method is to be more convenient for tests.
  */
  mpz_class getUpperBound(const Term& divisor,
                          const Term& dominator) const;

  /** Returns the index of the largest stored exponent of var that is
   less than value. If strict is true, then it is strictly less than,
   otherwise it is less than or equal to. If no such exponent exists,
   then the return value is 0.

   If the sign is zero, then 0 is returned. Uses a linear scan, and is
   only really used to check the output of the other overload.
  */
  Exponent getLargestLessThan2(size_t var,
                               const mpz_class& value, bool strict = true) const;

  /** Returns the value in the interval [from, to] that is closest to
   getLargestLessThan(var, value, strict). Only uses a logarithmic
   number of comparisons.
  */
  Exponent getLargestLessThan2(size_t var, Exponent from, Exponent to,
                              const mpz_class& value, bool strict =
                              true) const;

  void getIncrementedDegree(const Term& term,
                            const Projection& projection,
                            mpz_class& degree) const;

  /** Finds minimal index in [from, to] to such that degree(t) <=
   maxDegree. Returns true if such a t exists, and false otherwise.
  */
  bool getMinIndexLessThan(size_t var,
                           Exponent from,
                           Exponent to,
                           Exponent& index,
                           const mpz_class& maxDegree) const;

  /** Finds maximal index in [from, to] to such that degree(t) <=
   maxDegree. Returns true if such a t exists, and false otherwise.
  */
  bool getMaxIndexLessThan(size_t var,
                           Exponent from,
                           Exponent to,
                           Exponent& index,
                           const mpz_class& maxDegree) const;


  const mpz_class& getGrade(size_t var, Exponent exponent) const;

  Exponent getMaxExponent(size_t var) const;

  size_t getVarCount() const;

  void print(ostream& out) const;

  /** Returns 1 if the grade strictly increases with the exponent of
   var, returns -1 if it strictly decreases, and returns 0 if changing
   the exponent has no impact on the grade.
  */
  int getGradeSign(size_t var) const;

private:
  vector<vector<mpz_class> > _grades;
  vector<int> _signs;
};

ostream& operator<<(ostream& out, const TermGrader& grader);

#endif
