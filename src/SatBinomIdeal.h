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

  /// Returns true if the generating set seems generic, i.e. no
  /// generator has a zero entry and the initial ideal is weakly
  /// generic.
  ///
  /// This concept of genericity comes from Scarf's theory of
  /// maximal lattice-free bodies, and this link is preserved if the
  /// generating set is a graded reverse-lex toric Grobner basis of a
  /// primitive vector.
  bool isGeneric() const;

  /// Returns true if there are generators a and b such that {0,a,a+b}
  /// {0,b,a+b} are both interior point-free bodies.
  void getDoubleTriangleCount(mpz_class& count) const;

  /// Returns true if the smallest body containing zero, a and b has a
  /// generator in its interior.
  bool isPointFreeBody(const vector<mpz_class>& a,
					   const vector<mpz_class>& b) const;

  /// Returns true if any generator, considered as an integer vector,
  /// is dominated by v.
  bool isDominating(const vector<mpz_class>& v) const;

  /// Returns true if v is a generator.
  bool isGenerator(const vector<mpz_class>& v) const;

  void projectVar(size_t var);

  SatBinomIdeal& operator=(const SatBinomIdeal& ideal);

 private:
  vector<vector<mpz_class> > _gens;
  VarNames _names;
};

ostream& operator<<(ostream& out, const SatBinomIdeal& ideal);

#endif
