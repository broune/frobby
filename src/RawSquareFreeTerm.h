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

namespace SquareFreeTermOps {
  inline bool isIdentity(const Word* a, Word* aEnd) {
	for (; a != aEnd; ++a)
	  if (*a != 0)
		return false;
	return true;
  }

  inline bool isIdentity(const Word* a, size_t varCount) {
	if (varCount == 0)
	  return true;
	while (true) {
	  if (*a != 0)
		return false;
	  if (varCount <= BitsPerWord)
		return true;
	  ++a;
	  varCount -= BitsPerWord;
	}
  }

  inline size_t getSizeOfSupport(const Word* a, size_t varCount) {
	if (varCount == 0)
	  return 0;
	size_t count = 0;
	while (true) {
	  Word word = *a;
	  // TODO: should be able to improve this set bit counting algorithm.
	  while (word != 0) {
		if (word & 1 != 0)
		  ++count;
		word >>= 1;
	  }

	  if (varCount <= BitsPerWord)
		return count;
	  ++a;
	  varCount -= BitsPerWord;
	}
  }

  inline size_t getWordCount(size_t varCount) {
	// Compute varCount / BitsPerWord rounded up. Special case for
	// varCount == 0 as formula has underflow issue for that input.
	if (varCount == 0)
	  return 0;
	else
	  return ((varCount - 1) / BitsPerWord) + 1;
  }

  inline void setToIdentity(Word* res, const Word* resEnd) {
	for (; res != resEnd; ++res)
	  *res = 0;
  }

  inline void setToIdentity(Word* res, size_t varCount) {
	for (; varCount >= BitsPerWord; ++res, varCount -= BitsPerWord)
	  *res = 0;
	if (varCount > 0)
	  *res = 0;
  }

  /** Sets all exponents of res to 1. */
  inline void setToAllVarProd(Word* res, size_t varCount) {
	for (; varCount >= BitsPerWord; ++res, varCount -= BitsPerWord)
	  *res = ~0;
	if (varCount > 0) {
	  const Word fullSupportWord = (((Word)1) << varCount) - 1;
	  *res = fullSupportWord;
	}
  }

  /** Returns identity term of varCount variables. Must deallocate
	  with deleteTerm(). */
  inline Word* newTerm(size_t varCount) {
	const size_t wordCount = getWordCount(varCount);
	Word* word = new Word[wordCount];
	setToIdentity(word, word + wordCount);
	return word;
  }

  /** Allocates and returns a term based on str. The returned term
   must be deallocated using deleteTerm. str is parsed as a list of
   exponent vector entries that must be '0' or '1'. The string can
   contain no other characters. The parsing is inefficient and is
   intended for constructing terms in testing. */
  Word* newTermParse(const char* str);

  /** Deletes term previously returned by newTerm(). Term can be null. */
  inline void deleteTerm(Word* term) {
	delete[] term;
  }

  inline bool divides(const Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
	  if ((*a & (~*b)) != 0)
		return false;
	return true;
  }

  inline bool lexLess(const Word* a, const Word* b, size_t varCount) {
	if (varCount == 0)
	  return false;
	while (true) {
	  if (*a != *b) {
		Word xorAB = (*a) ^ (*b);
		ASSERT(xorAB != 0);
		Word leastSignificantBit = xorAB & (-xorAB);
		return (*a & leastSignificantBit) == 0;		
	  }
	  if (varCount <= BitsPerWord)
		return false; // a and b are equal
	  ++a;
	  ++b;
	  varCount -= BitsPerWord;
	}
  }

  inline void colon(Word* res, const Word* resEnd, const Word* a, const Word* b) {
	for (; res != resEnd; ++res, ++a, ++b)
	  *res = (*a) & (~*b);
  }

  inline void assign(Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
	  *a = *b;
  }

  inline void assign(Word* a, const Word* b, size_t varCount) {
	for (; varCount >= BitsPerWord; ++a, ++b, varCount -= BitsPerWord)
	  *a = *b;
	if (varCount > 0)
	  *a = *b;
  }

  /** Assigns the RawSquareFreeTerm-encoded form of term to encoded
	  and returns true if term is square free. Otherwise returns
	  false. */
  inline bool encodeTerm(Word* encoded, const Exponent* term, const size_t varCount) {
	size_t var = 0;
	while (var < varCount) {
	  Word bit = 1;
	  *encoded = 0;
	  do {
		if (term[var] == 1)
		  *encoded |= bit;
		else if (term[var] != 0)
		  return false;
		bit <<= 1;
		++var;
	  } while (bit != 0 && var < varCount);
	  ++encoded;
	}
	return true;
  }

  inline size_t getBitOffset(size_t var) {
	return var % BitsPerWord;
  }

  inline size_t getWordOffset(size_t var) {
	return var / BitsPerWord;
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

  inline void lcm(Word* res, const Word* resEnd,
				  const Word* a, const Word* b) {
	for (; res != resEnd; ++a, ++b, ++res)
	  *res = (*a) | (*b);
  }

  inline void lcm(Word* res, const Word* a, const Word* b, size_t varCount) {
	for (; varCount >= BitsPerWord; ++a, ++b, ++res, varCount -= BitsPerWord)
	  *res = (*a) | (*b);
	if (varCount != 0)
	  *res = (*a) | (*b);
  }

  inline void lcmInPlace(Word* res, const Word* resEnd, const Word* a) {
	for (; res != resEnd; ++a, ++res)
	  *res |= *a;
  }

  inline void lcmInPlace(Word* res, const Word* a, size_t varCount) {
	for (; varCount >= BitsPerWord; ++a, ++res, varCount -= BitsPerWord)
	  *res |= *a;
	if (varCount != 0)
	  *res |= *a;
  }

  inline void gcd(Word* res, const Word* resEnd,
				  const Word* a, const Word* b) {
	for (; res != resEnd; ++a, ++b, ++res)
	  *res = (*a) & (*b);
  }

  inline void gcdInPlace(Word* res, const Word* resEnd, const Word* a) {
	for (; res != resEnd; ++a, ++res)
	  *res &= *a;
  }

  inline void gcdInPlace(Word* res, const Word* a, size_t varCount) {
	for (; varCount >= BitsPerWord; ++a, ++res, varCount -= BitsPerWord)
	  *res &= *a;
	if (varCount != 0)
	  *res &= *a;
  }

  inline bool isRelativelyPrime(const Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
	  if ((*a) & (*b))
		return false;
	return true;
  }

  inline bool isRelativelyPrime(const Word* a, const Word* b, size_t varCount) {
	for (; varCount >= BitsPerWord; ++a, ++b, varCount -= BitsPerWord)
	  if ((*a) & (*b))
		return false;
	if (varCount != 0)
	  if ((*a) & (*b))
		return false;
	return true;
  }

  inline void setExponent(Word* a, size_t var, bool value) {
	Word& word = a[getWordOffset(var)];
	const size_t bitOffset = getBitOffset(var);
	const Word setBit = ((Word)1) << bitOffset;
	const Word valueBit = ((Word)value) << bitOffset;
	word = (word & (~setBit)) | valueBit;
  }

  /** Returns zero if var divides a and returns a non-zero value
	  otherwise. The non-zero value is not necessarily 1. */
  inline int varDivides(const Word* a, size_t var) {
	const Word word = a[getWordOffset(var)];
	const Word bitMask = ((Word)1) << getBitOffset(var);
	const Word value = word & bitMask;  // clear other bits
	return value;
  }

  /** returns true if var divides a and false otherwise. */
  inline bool getExponent(const Word* a, size_t var) {
	return varDivides(a, var) != 0;
  }

  inline void swap(Word* a, Word* b, size_t varCount) {
	for (; varCount >= BitsPerWord; ++a, ++b, varCount -= BitsPerWord)
	  std::swap(*a, *b);
	if (varCount > 0)
	  std::swap(*a, *b);
  }

  /** Make 0 exponents 1 and make 1 exponents 0. */
  inline void invert(Word* a, size_t varCount) {
	for (; varCount >= BitsPerWord; ++a, varCount -= BitsPerWord)
	  *a = ~*a;
	if (varCount > 0) {
	  const Word fullSupportWord = (((Word)1) << varCount) - 1;
	  *a = (~*a) & fullSupportWord;
	}
  }

  /** For every variable var that divides a, increment inc[var] by one. */
  inline void incrementAtSupport(const Word* a, size_t* inc, size_t varCount) {
	if (varCount == 0)
	  return;
	while (true) {
	  Word word = *a;
	  size_t* intraWordInc = inc;
	  while (word != 0) {
		*intraWordInc += word & 1; // +1 if bit set
		++intraWordInc;
		word = word >> 1;
	  }
	  if (varCount <= BitsPerWord)
		return;
	  varCount -= BitsPerWord;
	  inc += BitsPerWord;
	  ++a;
	}
  }

  /** Returns true if a equals b. */
  inline bool equals(const Word* a, const Word* b, size_t varCount) {
	if (varCount == 0)
	  return true;
	while (true) {
	  if (*a != *b)
		return false;
	  if (varCount <= BitsPerWord)
		return true;
	  ++a;
	  ++b;
	  varCount -= BitsPerWord;
	}
  }

  /** The unused bits at the end of the last word must be zero for the
	  functions here to work correctly. They should all maintain this
	  invariant. This function is useful in tests to ensure that that
	  is true. */
  inline bool isValid(Word* a, size_t varCount) {
	size_t offset = varCount / BitsPerWord;
	a += offset;
	varCount -= BitsPerWord * offset;
	if (varCount == 0)
	  return true;
	const Word fullSupportWord = (((Word)1) << varCount) - 1;
	return ((*a) & ~fullSupportWord) == 0;
  }

  void print(FILE* file, const Word* term, size_t varCount);
  void print(ostream& out, const Word* term, size_t varCount);
}

#endif
