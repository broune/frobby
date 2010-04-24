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
#ifndef RAW_SQUARE_FREE_IDEAL_GUARD
#define RAW_SQUARE_FREE_IDEAL_GUARD

class Ideal;

/** @todo describe further.

	The "raw" prefix is there for a reason!

	This class allocates none of its own memory. It must be
	initialized using the member function construct() in a buffer of
	sufficient size. What is sufficient memory for a given ideal can
	be determined by getBytesOfMemoryFor(). The buffer must be
	correctly aligned to support the data members that this class has.
*/
class RawSquareFreeIdeal {
 public:
  static RawSquareFreeIdeal* construct(void* buffer, size_t varCount = 0);
  static RawSquareFreeIdeal* construct(void* buffer, const Ideal& ideal);

  /** Returns the number of bytes of memory necessary to contain an
	  ideal with the given parameters. This size is padded to ensure
	  proper alignment if ideals are placed one after the other.

	  This function throws bad_alloc if the number of bytes cannot be
	  represented in a size_t, or nearly cannot be represented in a
	  size_t. That is proper for most any circumstance where a call to
	  this function appers, but if that is not what you want you will
	  need to catch the bad_alloc. This function will not throw
	  bad_alloc for any other reason than that.
  */
  static size_t getBytesOfMemoryFor(size_t varCount, size_t generatorCount);

  RawSquareFreeIdeal& operator=(const RawSquareFreeIdeal& ideal);

  void minimize();
  void insert(const Word* term);
  void colon(const Word* by);

  void insertReminimize(const Word* term);
  void colonReminimize(const Word* colon);

  void getLcm(Word* lcm) const;

  static RawSquareFreeIdeal* newIdeal(size_t varCount, size_t capacity);
  static void deleteIdeal(RawSquareFreeIdeal* ideal);

  size_t getGeneratorCount() const;
  size_t getVarCount() const;
  size_t getWordsPerTerm() const;

  Word* getGenerator(size_t index);
  const Word* getGenerator(size_t index) const;

  /** Sets counts[var] to the number of generators that var divides. */
  void getVarDividesCounts(size_t* counts) const;

  void remove(Word* term);
  Word* getNotRelativelyPrime(const Word* term);

 private:
  // Not available
  RawSquareFreeIdeal();
  RawSquareFreeIdeal(const RawSquareFreeIdeal&);

  size_t _varCount;
  size_t _genCount;
  Word _memory[1]; // variable size array
};

/** Allocates object with enough memory for capacity generators in
	varCount variables. Pointer must be deallocated using
	deleteRawSquareFreeIdeal. */
RawSquareFreeIdeal* newRawSquareFreeIdeal(size_t varCount, size_t capacity);

/** Deallocates memory returned by newRawSquareFreeIdeal(). */
void deleteRawSquareFreeIdeal(RawSquareFreeIdeal* ideal);

#endif
