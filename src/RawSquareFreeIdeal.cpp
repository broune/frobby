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
#include "stdinc.h"
#include "RawSquareFreeIdeal.h"

#include "Ideal.h"
#include "RawSquareFreeTerm.h"

#include <limits>
#include <algorithm>

typedef RawSquareFreeIdeal RSFIdeal;
namespace Ops = SquareFreeTermOps;

RSFIdeal* RSFIdeal::construct(void* buffer, size_t varCount) {
  RSFIdeal* p = static_cast<RSFIdeal*>(buffer);
  p->_varCount = varCount;
  p->_genCount = 0;
  return p;
}

RSFIdeal* RSFIdeal::construct(void* buffer, const Ideal& ideal) {
  RSFIdeal* p = construct(buffer, ideal.getVarCount());

  for (size_t gen = 0; gen < ideal.getGeneratorCount(); ++gen) {
	p->_genCount += 1;
	Word* packed = p->getGenerator(p->_genCount - 1);

	size_t var = 0;
	while (var < ideal.getVarCount()) {
	  Word bit = 1;
	  *packed = 0;
	  do {
		if (ideal[gen][var] > 0)
		  *packed |= bit;
		bit <<= 1;
		++var;
	  } while (bit != 0 && var < ideal.getVarCount());
	  ++packed;
	}
  }
  return p;
}

size_t RSFIdeal::getBytesOfMemoryFor(size_t varCount, size_t generatorCount) {
  // Note that x / y rounded up is (x - 1) / y + 1 for x, y > 0. The
  // multiplication a * b does not overflow if a <= MAX /
  // b. Otherwise, there may not be an overflow, but a * b will still be
  // too much to reasonably allocate.

  size_t bytesForStruct = sizeof(RSFIdeal) - sizeof(Word);

  // Compute bytes per generator taking into account memory alignment.
  size_t bytesPerGenUnaligned = (varCount - 1) / 8 + 1;
  size_t wordsPerGen = (bytesPerGenUnaligned - 1) / sizeof(Word) + 1;
  if (wordsPerGen > numeric_limits<size_t>::max() * sizeof(Word))
	throw bad_alloc();
  size_t bytesPerGen = wordsPerGen * sizeof(Word);

  // Compute bytes in all.
  if (bytesPerGen > numeric_limits<size_t>::max() / generatorCount)
	throw bad_alloc();
  size_t bytesForGens = bytesPerGen * generatorCount;
  if (bytesForGens > numeric_limits<size_t>::max() - bytesForStruct)
	throw bad_alloc();
  return bytesForStruct + bytesForGens;
}

RSFIdeal& RSFIdeal::operator=(const RSFIdeal& ideal) {
  _varCount = ideal._varCount;
  _genCount = 0;
  for (size_t gen = 0; gen < ideal.getGeneratorCount(); ++gen)
	insert(ideal.getGenerator(gen));
  return *this;
}

void RSFIdeal::minimize() {
  size_t wordCount = getWordsPerTerm();
  for (size_t i = 0; i < _genCount; ++i) {
	for (size_t div = 0; div < _genCount; ++div) {
	  if (div != i &&
		  Ops::divides(getGenerator(div), getGenerator(div) + wordCount,
					   getGenerator(i))) {
		--_genCount;
		Ops::assign(getGenerator(i), getGenerator(i) + wordCount,
					getGenerator(_genCount));
		goto next;
	  }
	}
  next:;
  }
}

void RSFIdeal::colon(const Word* by) {
  size_t wordCount = getWordsPerTerm();
  for (size_t i = 0; i < _genCount; ++i)
	Ops::colon(getGenerator(i), getGenerator(i) + wordCount,
			   getGenerator(i), by);
}

Word* RSFIdeal::getGenerator(size_t index) {
  return _memory + index * getWordsPerTerm();
}

const Word* RSFIdeal::getGenerator(size_t index) const {
  return _memory + index * getWordsPerTerm();
}

void RSFIdeal::getVarDividesCounts(size_t* counts) const {
  fill_n(counts, getVarCount(), static_cast<size_t>(0));
  size_t wordCount = getWordsPerTerm();
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen) {
	size_t* countIt = counts;
	for (size_t word = 0; word < wordCount; ++word) {
	  Word bit = 1;
	  // countIt can go beyond counts + getVarCount here, since it
	  // will go to the end of the word, but the entries there will
	  // all be 0 so countIt will not be accessed when it is invalid,
	  // so there is not a problem. It may be more efficient to
	  // compute a countEnd outside the loop and compare to that
	  // instead of the bit. That would have to come down to an
	  // experiment.
	  //
	  // TODO: actually this is not OK, since C++ does not support
	  // even calculating pointers past the end of an array, except
	  // for the first spot past the end.
	  do {
		if ((getGenerator(gen)[word] & bit) != 0) {
		  ASSERT(countIt < counts + getVarCount());
		  *countIt += 1;
		}
		++countIt;
	  } while (bit != 0);	  
	}
  }
}

void RSFIdeal::remove(Word* term) {
  // todo: assert term is valid
  Word* last = getGenerator(getGeneratorCount() - 1);
  if (term != last)
	Ops::assign(term, term + getWordsPerTerm(), last);
  --_genCount;
}

Word* RSFIdeal::getNotRelativelyPrime(const Word* term) {
  const size_t wordCount = getWordsPerTerm();
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
	if (!Ops::isRelativelyPrime(term, term + wordCount, getGenerator(gen)))
	  return getGenerator(gen);
  return 0;
}

void RSFIdeal::insert(const Word* term) {
  ++_genCount;
  Word* pos = getGenerator(_genCount - 1);
  Ops::assign(pos, pos + getWordsPerTerm(), term);
}

size_t RSFIdeal::getGeneratorCount() const {
  return _genCount;
}

size_t RSFIdeal::getVarCount() const {
  return _varCount;
}

size_t RSFIdeal::getWordsPerTerm() const {
  ASSERT(_varCount > 0);
  return Ops::getWordCount(_varCount);
}

void RSFIdeal::insertReminimize(const Word* term) {
  insert(term);
  minimize();
}

void RSFIdeal::colonReminimize(const Word* by) {
  colon(by);
  minimize();
}

void RSFIdeal::getLcm(Word* lcm) const {
  const size_t wordCount = getWordsPerTerm();
  Ops::setToIdentity(lcm, lcm + wordCount);
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
	Ops::lcm(lcm, lcm + wordCount, lcm, getGenerator(gen));
}

RSFIdeal* newRawSquareFreeIdeal(size_t varCount, size_t capacity) {
  size_t byteCount = RSFIdeal::getBytesOfMemoryFor(varCount, capacity);
  void* buffer = new char[byteCount];
  return RSFIdeal::construct(buffer, varCount);
}

void deleteRawSquareFreeIdeal(RSFIdeal* ideal) {
  delete[] ideal;
}
