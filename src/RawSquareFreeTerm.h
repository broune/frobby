/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#ifndef RAW_SQUARE_FREE_TERM_GUARD
#define RAW_SQUARE_FREE_TERM_GUARD

#include <ostream>
#include <algorithm>
#include <vector>

namespace SquareFreeTermOps {
  bool isIdentity(const Word* a, Word* aEnd);

  bool isIdentity(const Word* a, size_t varCount);

  size_t getSizeOfSupport(const Word* a, size_t varCount);

  size_t getWordCount(size_t varCount);

  /** For every variable var that divides remove, remove the space for
   that variable in term and put the result in compacted. So the
   operation performed is a colon, except that the bits are removed
   instead of just set to zero. The remaining bits are moved into the
   gaps so that compacted may take up less space than term does. The
   relative order of the remaining variables is preserved. The result
   is placed in res.

   It is allowed for compacted to equal term. Otherwise its memory
   must be disjoint from that of term. The memory for remove must
   always be disjoint from that of term. */
  void compact(Word* compacted, const Word* term,
			   const Word* remove, size_t varCount);

  void setToIdentity(Word* res, const Word* resEnd);

  void setToIdentity(Word* res, size_t varCount);

  /** Sets all exponents of res to 1. */
  void setToAllVarProd(Word* res, size_t varCount);

  /** Returns identity term of varCount variables. Must deallocate
	  with deleteTerm(). */
  Word* newTerm(size_t varCount);

  /** Allocates and returns a term based on str. The returned term
   must be deallocated using deleteTerm. str is parsed as a list of
   exponent vector entries that must be '0' or '1'. The string can
   contain no other characters. The parsing is inefficient and is
   intended for constructing terms in testing. */
  Word* newTermParse(const char* str);

  /** Deletes term previously returned by newTerm(). Term can be null. */
  void deleteTerm(Word* term);

  /** Returns true if a divides b. */
  bool divides(const Word* a, const Word* aEnd, const Word* b);

  bool lexLess(const Word* a, const Word* b, size_t varCount);

  void colon(Word* res, const Word* resEnd, const Word* a, const Word* b);
  void colonInPlace(Word* res, const Word* resEnd, const Word* b);

  void assign(Word* a, const Word* aEnd, const Word* b);

  void assign(Word* a, const Word* b, size_t varCount);

  /** Assigns the RawSquareFreeTerm-encoded form of term to encoded
	  and returns true if term is square free. Otherwise returns
	  false. */
  bool encodeTerm(Word* encoded, const Exponent* term, const size_t varCount);
  bool encodeTerm(Word* encoded, const std::vector<mpz_class>& term, const size_t varCount);
  bool encodeTerm(Word* encoded, const std::vector<std::string>& it, const size_t varCount);

  inline size_t getBitOffset(size_t var);

  inline size_t getWordOffset(size_t var);

  bool hasFullSupport(const Word* a, size_t varCount);

  void lcm(Word* res, const Word* resEnd,
				  const Word* a, const Word* b);

  void lcm(Word* res, const Word* a, const Word* b, size_t varCount);

  void lcmInPlace(Word* res, const Word* resEnd, const Word* a);

  void lcmInPlace(Word* res, const Word* a, size_t varCount);

  void gcd(Word* res, const Word* resEnd, const Word* a, const Word* b);
  void gcd(Word* res, const Word* a, const Word* b, size_t varCount);

  void gcdInPlace(Word* res, const Word* resEnd, const Word* a);

  void gcdInPlace(Word* res, const Word* a, size_t varCount);

  bool isRelativelyPrime(const Word* a, const Word* aEnd, const Word* b);

  bool isRelativelyPrime(const Word* a, const Word* b, size_t varCount);

  void setExponent(Word* a, size_t var, bool value);

  /** returns true if var divides a and false otherwise. */
  bool getExponent(const Word* a, size_t var);

  void swap(Word* a, Word* b, size_t varCount);
  void swap(Word* a, Word* aEnd, Word* b);

  /** Make 0 exponents 1 and make 1 exponents 0. */
  void invert(Word* a, size_t varCount);

  /** Returns var if a equals var. If a is the identity or the product
	  of more than one variable then returns varCount. */
  size_t getVarIfPure(const Word* const a, size_t varCount);

  /** For every variable var that divides a, decrement inc[var] by one. */
  void decrementAtSupport(const Word* a, size_t* inc, size_t varCount);

  /** For every variable var that divides a, set inc[var] to zero. */
  void toZeroAtSupport(const Word* a, size_t* inc, size_t varCount);

  /** Returns true if a equals b. */
  bool equals(const Word* a, const Word* b, size_t varCount);

  /** The unused bits at the end of the last word must be zero for the
	  functions here to work correctly. They should all maintain this
	  invariant. This function is useful in tests to ensure that that
	  is true. */
  bool isValid(const Word* a, size_t varCount);

  inline bool divides(const Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
	  if ((*a & (~*b)) != 0)
		return false;
	return true;
  }

  inline bool getExponent(const Word* a, size_t var) {
	const Word word = a[getWordOffset(var)];
	const Word bitMask = ((Word)1) << getBitOffset(var);
	return word & bitMask;
  }

  inline size_t getBitOffset(size_t var) {
	return var % BitsPerWord;
  }

  inline size_t getWordOffset(size_t var) {
	return var / BitsPerWord;
  }

  inline void assign(Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
	  *a = *b;
  }

  inline void setExponent(Word* a, size_t var, bool value) {
	Word& word = a[getWordOffset(var)];
	const size_t bitOffset = getBitOffset(var);
	const Word setBit = ((Word)1) << bitOffset;
	const Word valueBit = ((Word)value) << bitOffset;
	word = (word & (~setBit)) | valueBit;
  }

  inline void swap(Word* a, Word* aEnd, Word* b) {
	for (; a != aEnd; ++a, ++b)
	  std::swap(*a, *b);
  }

  inline bool hasFullSupport(const Word* a, size_t varCount) {
	const Word allOnes = ~((Word)0);
	for (; varCount >= BitsPerWord; varCount -= BitsPerWord, ++a)
	  if (*a != allOnes)
		return false;
	if (varCount == 0)
	  return true;

	const Word fullSupportWord = (((Word)1) << varCount) - 1;
	return *a == fullSupportWord;
  }

  inline bool isRelativelyPrime(const Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
	  if ((*a) & (*b))
		return false;
	return true;
  }

  void print(FILE* file, const Word* term, size_t varCount);
  void print(ostream& out, const Word* term, size_t varCount);
}

#endif
