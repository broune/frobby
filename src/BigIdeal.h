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
#ifndef BIG_IDEAL_GUARD
#define BIG_IDEAL_GUARD

#include "VarNames.h"
#include <vector>

class TermTranslator;
class Ideal;
class SquareFreeIdeal;

class BigIdeal {
public:
  BigIdeal();
  BigIdeal(const VarNames& names);

  void insert(const Ideal& ideal);
  void insert(const Ideal& ideal, const TermTranslator& translator);
  void insert(const SquareFreeIdeal& ideal);
  void insert(const vector<mpz_class>& term);

  void renameVars(const VarNames& names);

  void newLastTerm();
  void reserve(size_t capacity);

  mpz_class& getLastTermExponentRef(size_t var);
  vector<mpz_class>& getLastTermRef();
  const vector<mpz_class>& getTerm(size_t term) const;

  void getLcm(vector<mpz_class>& lcm) const;

  const mpz_class& getExponent(size_t term, size_t var) const;
  mpz_class& getExponent(size_t term, size_t var);
  void setExponent(size_t term, size_t var, const mpz_class& exp);

  bool operator==(const BigIdeal& b) const;

  vector<mpz_class>& operator[](size_t index);
  const vector<mpz_class>& operator[](size_t index) const;

  void projectVar(size_t var);

  // This also depends on the order of the variables.
  bool operator<(const BigIdeal& ideal) const;

  bool empty() const;
  bool containsIdentity() const;
  bool contains(const vector<mpz_class>& term) const;

  void clear();

  inline size_t getGeneratorCount() const;
  size_t getVarCount() const;

  void clearAndSetNames(const VarNames& names);

  // Adds a variable to the VarNames associated to the ideal. It is an
  // error to call this method when the ideal has any generators. If
  // var is already a known variable, nothing is changed, and the
  // return value is false. Otherwise, the return value is true.
  bool addVarToClearedIdeal(const char* var);

  // Remove variable from each generator and from the ring.
  void eraseVar(size_t var);

  const VarNames& getNames() const;

  // Applies a generic deformation.
  void deform();

  // Takes the radical of every generator.
  void takeRadical();

  // Sorts the generators and removes duplicates.
  void sortGeneratorsUnique();

  // Sorts the generators.
  void sortGenerators();

  // Sorts the variables.
  void sortVariables();

  void swap(BigIdeal& ideal);

  void print(FILE* file) const;
  void print(ostream& out) const;

  static bool bigTermCompare(const vector<mpz_class>& a,
                             const vector<mpz_class>& b);
private:
  vector<vector<mpz_class> > _terms;
  VarNames _names;
};

ostream& operator<<(ostream& out, const BigIdeal& ideal);
ostream& operator<<(ostream& out, const vector<BigIdeal>& ideals);



inline vector<mpz_class>& BigIdeal::operator[](size_t index) {
  ASSERT(index < _terms.size());
  return _terms[index];
}

inline const vector<mpz_class>& BigIdeal::operator[](size_t index) const {
  ASSERT(index < _terms.size());
  return _terms[index];
}

inline mpz_class& BigIdeal::getLastTermExponentRef(size_t var) {
  ASSERT(!empty());
  ASSERT(var < _names.getVarCount());

  return _terms.back()[var];
}

inline vector<mpz_class>& BigIdeal::getLastTermRef() {
  ASSERT(!empty());

  return _terms.back();
}

inline const vector<mpz_class>& BigIdeal::getTerm(size_t term) const {
  ASSERT(term < getGeneratorCount());
  return _terms[term];
}

inline size_t BigIdeal::getGeneratorCount() const {
  return _terms.size();
}

inline size_t BigIdeal::getVarCount() const {
  return _names.getVarCount();
}

#endif
