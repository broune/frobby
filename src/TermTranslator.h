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
#ifndef TERM_TRANSLATOR_GUARD
#define TERM_TRANSLATOR_GUARD

#include "VarNames.h"

#include <vector>
#include <ostream>

class BigIdeal;
class Ideal;
class Term;

/** TermTranslator handles translation between terms whose exponents
 are infinite precision integers and terms whose exponents are 32
 bit integers.

 This is done by assigning the big integers IDs that are 32 bit
 integers such that the assignment of IDs preserves order of
 exponents for each variable.

 The translation is done at the level of whole ideals.

 The big integer 0 is always assigned the ID 0.
*/
class TermTranslator {
public:
  /** Constructs a translator of varCount variables that translates each
   number to itself, up to and not including upToExponent.
  */
  TermTranslator(size_t varCount, size_t upToExponent);

  /** Translates bigIdeal into ideal, and construct a translator to
   translate back. sortVars indicates whether or not the order of the
   variable names should be sorted.
  */
  TermTranslator(const BigIdeal& bigIdeal, Ideal& ideal, bool sortVars = true);

  /** Translates bigIdeals into ideals, while constructing a
   translator to translate back. The variable names will be sorted,
   and the ideals will be embedded in a ring with the union of all
   variables present in bigIdeals.
  */
  TermTranslator(const vector<BigIdeal*>& bigIdeals, vector<Ideal*>& ideals);

  TermTranslator(const TermTranslator& translator);
  ~TermTranslator();

  TermTranslator& operator=(const TermTranslator& translator);

  /** This method translates from IDs to arbitrary precision integers.
  */
  const mpz_class& getExponent(size_t variable, Exponent exponent) const;

  /** This method translates from IDs to arbitrary precision integers.
  */
  const mpz_class& getExponent(size_t variable, const Term& term) const;

  /** As getExponent, except the string "var^e" is returned or null if
   the exponent is zero, where var is the variable and e is the
   exponent.
  */
  const char* getVarExponentString(size_t variable, Exponent exponent) const;

  /** as getExponent, except the string "e" is returned, where e is the
   exponent.
  */
  const char* getExponentString(size_t variable, Exponent exponent) const;

  /** The assigned IDs are those in the range [0, getMaxId(var)]. As a
      special case, getMaxId(var) maps to the same exponent as 0
      does. */
  Exponent getMaxId(size_t variable) const;

  /** Adds a generator of the form v^e, e > 0, for any variable v where
   generator of that form is not already present. e is chosen to be
   larger than any exponent (i.e. ID) already present, and it maps
   to 0. Note that this does NOT preserve order - the highest ID
   always maps to 0. The reason for this is that this is what is
   needed for computing irreducible decompositions.
  */
  void addPurePowersAtInfinity(Ideal& ideal) const;

  /** The method addPurePowersAtInfinity adds high exponents that map to
   zero. This method replaces those high powers with the power
   zero.
  */
  void setInfinityPowersToZero(Ideal& ideal) const;

  const VarNames& getNames() const;
  size_t getVarCount() const;

  /** Replaces var^v by var^(a[i] - v) except that var^0 is left
   alone.
  */
  void dualize(const vector<mpz_class>& a);

  /** Replaces var^v by var^(v-1). */
  void decrement();

  void renameVariables(const VarNames& names);
  void swapVariables(size_t a, size_t b);

  bool lessThanReverseLex(const Exponent* a, const Exponent* b) const;

  void print(ostream& out) const;
  string toString() const;

private:
  void makeStrings(bool includeVar) const;
  void clearStrings();

  void initialize(const vector<BigIdeal*>& bigIdeals, bool sortVars);
  void shrinkBigIdeal(const BigIdeal& bigIdeal, Ideal& ideal) const;
  Exponent shrinkExponent(size_t var, const mpz_class& exponent) const;

  vector<vector<mpz_class> > _exponents;
  mutable vector<vector<const char*> > _stringExponents;
  mutable vector<vector<const char*> > _stringVarExponents;
  VarNames _names;
};

/** A predicate that sorts according to reverse lexicographic order
 on the translated values of a term.
*/
class TranslatedReverseLexComparator {
 public:
 TranslatedReverseLexComparator(const TermTranslator& translator):
  _translator(translator) {
  }

  bool operator()(const Term& a, const Term& b) const;
  bool operator()(const Exponent* a, const Exponent* b) const;

 private:
  const TermTranslator& _translator;
};

void setToZeroOne(TermTranslator& translator);

ostream& operator<<(ostream& out, const TermTranslator& translator);

#endif
