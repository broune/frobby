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

#include <ostream>
#include <vector>

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

	  todo: change the exception behavior to no exceptions.
  */
  static size_t getBytesOfMemoryFor(size_t varCount, size_t generatorCount);

  RawSquareFreeIdeal& operator=(const RawSquareFreeIdeal& ideal);

  void print(FILE* file) const;
  void print(ostream& out) const;

  /** Inserts the generators of ideal from index 0 onward until
	  reaching a non-squarefree generator or all generators have been
	  inserted. Returns the number of generators that has been
	  inserted. There must be enough capacity for all the generators
	  even if not all are actually inserted. */
  size_t insert(const Ideal& ideal);

  void minimize();
  void insert(const Word* term);
  void colon(const Word* by);

  /** Insert term and minimize. Object msut be already minimized. */
  void insertReminimize(const Word* term);

  /** Perform a colon and minimize. Object must be already minimized. */
  void colonReminimize(const Word* colon);

  /** Remove the generator at index, then perform a colon by it and
	  minimize. Object must be already minimized. */
  void colonReminimizeAndRemove(size_t index);

  void getLcm(Word* lcm) const;

  static RawSquareFreeIdeal* newIdeal(size_t varCount, size_t capacity);
  static void deleteIdeal(RawSquareFreeIdeal* ideal);

  size_t getGeneratorCount() const;
  size_t getVarCount() const;
  size_t getWordsPerTerm() const;

  Word* getGenerator(size_t index);
  const Word* getGenerator(size_t index) const;

  /** Sets counts[var] to the number of generators that var divides. */
  void getVarDividesCounts(vector<size_t>& counts) const;

  void removeGenerator(size_t index);
  Word* getNotRelativelyPrime(const Word* term);

  /** Returns the index of a generator that is the only one to be
	  divisible by some variable. Returns getGeneratorCount() if there
	  is no such generator. */
  size_t getExclusiveVarGenerator();

  /** Returns true if for every variable it either divides ignore or
	  it divides some (not necessarily minimal) generator. */
  bool hasFullSupport(const Word* ignore) const;

  void swap(size_t a, size_t b);

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

inline ostream& operator<<(ostream& out, const RawSquareFreeIdeal& ideal) {
  ideal.print(out);
  return out;
}

#endif
