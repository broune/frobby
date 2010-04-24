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
#ifndef RAW_SQUARE_FREE_TERM
#define RAW_SQUARE_FREE_TERM

namespace SquareFreeTermOps {
  inline size_t getWordCount(size_t varCount) {
	if (varCount == 0)
	  return 0;
	else
	  return ((varCount - 1) / BitsPerWord) + 1; // divison rounding up
  }

  /** Returns identity term of varCount variables. Must deallocate
	  with deleteTerm(). */
  inline Word* newTerm(size_t varCount) {
	return new Word[getWordCount(varCount)](); // zeroed due to ()
  }

  /** Deletes term previously returned by newTerm(). */
  inline void deleteTerm(Word* term) {
	delete[] term;
  }

  inline bool divides(const Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
	  if ((*a & (~*b)) != 0)
		return false;
	return true;
  }

  inline void colon(Word* res, const Word* resEnd, const Word* a, const Word* b) {
	for (; res != resEnd; ++res, ++a, ++b)
	  *res = (*a) & (~*b);
  }

  inline void assign(Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
	  *a = *b;
  }

  inline size_t getBitOffset(size_t var) {
	return var % BitsPerWord;
  }

  inline size_t getWordOffset(size_t var) {
	return var / BitsPerWord;
  }

  inline bool hasFullSupport(const Word* a, const Word* aEnd, size_t varCount) {
	// todo: find a way to not use wordCount here.
	size_t wordCount = getWordCount(varCount);
	const Word allOnes = ~((Word)0);
	for (; a != aEnd; ++a) {
	  if (*a != allOnes) {
		if (a == aEnd - 1 && varCount != wordCount * BitsPerWord) {
		  const size_t offset = getBitOffset(varCount);
		  if (*a == (((Word)1) << offset) - 1)
			return true;
		}
		return false;
	  }
	}
	return true;
  }

  inline void lcm(Word* res, const Word* resEnd,
				  const Word* a, const Word* b) {
	for (; res != resEnd; ++a, ++b, ++res)
	  *res = (*a) | (*b);
  }

  inline void gcd(Word* res, const Word* resEnd,
				  const Word* a, const Word* b) {
	for (; res != resEnd; ++a, ++b, ++res)
	  *res = (*a) & (*b);
  }

  inline bool isRelativelyPrime(const Word* a, const Word* aEnd, const Word* b) {
	for (; a != aEnd; ++a, ++b)
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

  inline bool getExponent(Word* a, size_t var) {
	const Word word = a[getWordOffset(var)];
	const Word bitMask = ((Word)1) << getBitOffset(var);
	const Word value = word & bitMask;  // clear other bits
	return value != 0;
  }

  inline bool isIdentity(Word* a, Word* aEnd) {
	for (; a != aEnd; ++a)
	  if (*a != 0)
		return false;
	return true;
  }

  inline void setToIdentity(Word* res, Word* resEnd) {
	for (; res != resEnd; ++res)
	  *res = 0;
  }
}

#endif
