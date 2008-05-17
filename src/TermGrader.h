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

class TermGrader {
public:
  TermGrader(const vector<mpz_class>& varDegrees,
	     const TermTranslator* translator);

  void getDegree(const Term& term, mpz_class& degree) const;

  void getDegree(const Term& term,
		 const Projection& projection,
		 mpz_class& degree) const;

  void getIncrementedDegree(const Term& term,
			    const Projection& projection,
			    mpz_class& degree) const;

  const mpz_class& getGrade(size_t var, Exponent exponent) const;

  Exponent getMaxExponent(size_t var) const;

  size_t getVarCount() const;

  void print(FILE* file) const;

private:
  vector<vector<mpz_class> > _grades;
};

#endif
