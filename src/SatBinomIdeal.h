/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef SAT_BINOM_IDEAL_GUARD
#define SAT_BINOM_IDEAL_GUARD

#include "VarNames.h"

#include <vector>

class BigIdeal;
class Matrix;

/// Represents a saturated binomial ideal.
class SatBinomIdeal {
 public:
  SatBinomIdeal();
  SatBinomIdeal(const VarNames& names);

  void insert(const vector<mpz_class>& binom);
  const vector<mpz_class>& getGenerator(size_t index) const;
  size_t getGeneratorCount() const;

  void print(FILE* file) const;
  void print(ostream& out) const;

  void clearAndSetNames(const VarNames& names);

  void removeGeneratorsWithLeadingZero();

  void removeGeneratorsWithoutLeadingZero();

  void getInitialIdeal(BigIdeal& ideal) const;

  /// Requires that names.getVarCount() equals getVarCount().
  void renameVars(const VarNames& names);

  size_t getVarCount() const;

  void clear();

  void reserve(size_t size);
  vector<mpz_class>& getLastBinomRef();
  void newLastTerm();

  const VarNames& getNames() const;

  /// Returns true if any generator does not involve every variable,
  /// i.e. the vector representing the generator has a zero entry.
  bool hasZeroEntry() const;

  /// Returns true if the initial ideal is weakly generic.
  bool initialIdealIsWeaklyGeneric() const;

  /// Returns true if the generating set is generic, i.e. no
  /// generator has a zero entry.
  ///
  /// This concept of genericity comes from Scarf's theory of
  /// maximal lattice-free bodies, and this link is preserved if the
  /// generating set is a graded reverse-lex toric Grobner basis of a
  /// primitive vector.
  bool isGeneric() const;

  /// Returns the number of pairs of generators a and b such that
  /// {0,a,a+b} and {0,b,a+b} are both interior point-free bodies.
  void getDoubleTriangleCount(mpz_class& count) const;

  /// Returns true if the smallest body containing zero, a and b has
  /// no generator in its interior.
  bool isPointFreeBody(const vector<mpz_class>& a,
                       const vector<mpz_class>& b) const;

  /// Returns true if the smallest body containing zero, a, b and c
  /// has no generator in its interior.
  bool isPointFreeBody(const vector<mpz_class>& a,
                       const vector<mpz_class>& b,
					   const vector<mpz_class>& c) const;

  /** Returns true if max(0,a,b) is strictly positive in every element. */
  bool isInterior(const vector<mpz_class>& a,
				  const vector<mpz_class>& b) const;

  /** Returns whether {to,to+from} is an interior edge of Top. */
  bool isInteriorEdge(size_t from, size_t to) const;

  /** Returns wehther {from,to+from} is an interior edge of Top and
	  also {to,to+from} is an edge of Top (not necessarily
	  interior). */
  bool isTerminatingEdge(size_t from, size_t to) const;

  /** Temporary. Returns whether the graph satisfies some conditions
	  that it should if some hypotheses are correct. */
  bool validate() const;

  /// Returns true if any generator, considered as an integer vector,
  /// is dominated by v.
  bool isDominating(const vector<mpz_class>& v) const;

  /// Returns true if v is a generator.
  bool isGenerator(const vector<mpz_class>& v) const;

  void projectVar(size_t var);

  SatBinomIdeal& operator=(const SatBinomIdeal& ideal);

  void getMatrix(Matrix& matrix) const;

 private:
  vector<vector<mpz_class> > _gens;
  VarNames _names;
};

ostream& operator<<(ostream& out, const SatBinomIdeal& ideal);

#endif
