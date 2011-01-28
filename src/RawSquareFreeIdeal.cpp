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
#include <sstream>

typedef RawSquareFreeIdeal RSFIdeal;
namespace Ops = SquareFreeTermOps;

RSFIdeal* RSFIdeal::construct(void* buffer, size_t varCount) {
  RSFIdeal* p = static_cast<RSFIdeal*>(buffer);
  p->_varCount = varCount;
  p->_wordsPerTerm = Ops::getWordCount(varCount);
  p->_genCount = 0;
  return p;
}

RSFIdeal* RSFIdeal::construct(void* buffer, const Ideal& ideal) {
  RSFIdeal* p = construct(buffer, ideal.getVarCount());
  p->insert(ideal);
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
  // todo: how does this multiplication not always overflow?
  if (wordsPerGen > numeric_limits<size_t>::max() * sizeof(Word))
	throw bad_alloc(); // todo: don't throw exceptions from get... function!
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
  _varCount = ideal.getVarCount();
  _wordsPerTerm = ideal.getWordsPerTerm();
  _genCount = 0;
  for (size_t gen = 0; gen < ideal.getGeneratorCount(); ++gen)
	insert(ideal.getGenerator(gen));
  return *this;
}

void RSFIdeal::print(FILE* file) const {
  ostringstream out;
  print(out);
  fputs(out.str().c_str(), file);
}

void RSFIdeal::print(ostream& out) const {
  out << "//------------ Ideal (Square Free):\n";
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen) {
    Ops::print(out, getGenerator(gen), getVarCount());
    out << '\n';
  }
  out << "------------\\\\\n";
}

size_t RSFIdeal::insert(const Ideal& ideal) {
  ASSERT(getVarCount() == ideal.getVarCount());

  size_t gen = 0;
  for (; gen < ideal.getGeneratorCount(); ++gen) {
	++_genCount;
	if (!Ops::encodeTerm(getGenerator(_genCount - 1),
						 ideal[gen], getVarCount())) {
	  --_genCount;
	  break;
	}
  }
  return gen;
}

void RSFIdeal::minimize() {
  size_t wordCount = getWordsPerTerm();
  const iterator start = begin();
  iterator stop = end();

  for (iterator it = start; it != stop;) {
	for (const_iterator div = start; div != stop; ++div) {
	  if (Ops::divides(*div, *div + wordCount, *it) && div != it) {
		--stop;
		Ops::assign(*it, *it + wordCount, *stop);
		--_genCount;
		goto next;
	  }
	}
	++it;
  next:;
  }
}

void RSFIdeal::colon(const Word* by) {
  const size_t wordCount = getWordsPerTerm();
  const iterator stop = end();
  for (iterator it = begin(); it != stop; ++it)
	Ops::colon(*it, *it + wordCount, *it, by);
}

void RSFIdeal::colon(size_t var) {
  const iterator stop = end();
  for (iterator it = begin(); it != stop; ++it)
	Ops::setExponent(*it, var, false);
}

Word* RSFIdeal::getGenerator(size_t index) {
  ASSERT(index < getGeneratorCount());
  return _memory + index * getWordsPerTerm();
}

const Word* RSFIdeal::getGenerator(size_t index) const {
  ASSERT(index < getGeneratorCount());
  return _memory + index * getWordsPerTerm();
}

Word* RSFIdeal::getGeneratorUnsafe(size_t index) {
  // no assert to check index is valid as this method specifically
  // allows out-of-bounds access.
  return _memory + index * getWordsPerTerm();
}

const Word* RSFIdeal::getGeneratorUnsafe(size_t index) const {
  // no assert to check index is valid as this method specifically
  // allows out-of-bounds access.
  return _memory + index * getWordsPerTerm();
}

void RSFIdeal::getLcmOfNonMultiples(Word* lcm, size_t var) const {
  ASSERT(var < getVarCount());

  const Word* const lcmEnd = lcm + getWordsPerTerm();
  Ops::setToIdentity(lcm, lcmEnd);

  const const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
	if (Ops::getExponent(*it, var) == 0)
	  Ops::lcmInPlace(lcm, lcmEnd, *it);
}

void RSFIdeal::getVarDividesCounts(vector<size_t>& counts) const {
  counts.resize(getVarCount());
  fill(counts.begin(), counts.end(), static_cast<size_t>(0));
  size_t* countsPtr = &(counts.front());

  const const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
	Ops::incrementAtSupport(*it, countsPtr, getVarCount());
}

void RSFIdeal::getVarDividesCounts(vector<size_t>& divCounts,
								   Word* termTmp) const {
  divCounts.resize(getVarCount());
  fill(divCounts.begin(), divCounts.end(), static_cast<size_t>(0));
  size_t* divCountsPtr = &(divCounts.front());
  const size_t varCount = getVarCount();

  // mask is 000100010001...0001 in binary.
  const static Word mask = ~0u / 15u;
  ASSERT(Ops::getSizeOfSupport(&mask, BitsPerWord) == BitsPerWord / 4);
  ASSERT(~0u == mask | (mask << 1) | (mask << 2) | (mask << 3));
  ASSERT(BitsPerWord % 4 == 0);

  size_t generatorsToGo = getGeneratorCount();
  const_iterator blockBegin = begin();
  const_iterator blockEnd = blockBegin;
  while (generatorsToGo > 0) {
	if (generatorsToGo >= 15) {
	  blockEnd = blockBegin + 15;
	  generatorsToGo -= 15;
	} else {
	  blockEnd = blockBegin + generatorsToGo;
	  generatorsToGo = 0;
	}

	const size_t wordCount = getWordsPerTerm();
	for (size_t i = 0; i < wordCount; ++i) {
	  Word w0 = 0;
	  Word w1 = 0;
	  Word w2 = 0;
	  Word w3 = 0;

	  for (const_iterator it = blockBegin; it != blockEnd; ++it) {
		const Word word = (*it)[i];
		w0 += word & mask;
		w1 += (word >> 1) & mask;
		w2 += (word >> 2) & mask;
		w3 += (word >> 3) & mask;
	  }

	  size_t var = i * BitsPerWord;
	  size_t* counts = divCountsPtr + var;
	  for (size_t j = 0; j < BitsPerWord / 4; ++j) {
		if (var < varCount) *counts += w0 & 0xF; else break;
		++var; ++counts;
		if (var < varCount) *counts += w1 & 0xF; else break;
		++var; ++counts;
		if (var < varCount) *counts += w2 & 0xF; else break;
		++var; ++counts;
		if (var < varCount) *counts += w3 & 0xF; else break;
		++var; ++counts;
		w0 >>= 4;
		w1 >>= 4;
		w2 >>= 4;
		w3 >>= 4;
	  }
	}
	blockBegin = blockEnd;
  }

#ifdef DEBUG
  vector<size_t> check;
  getVarDividesCounts(check);
  ASSERT(check == divCounts);
#endif
}

size_t RSFIdeal::getMultiple(size_t var) const {
  ASSERT(var < getVarCount());

  const const_iterator stop = end();
  const const_iterator start = begin();
  for (const_iterator it = start; it != stop; ++it)
	if (Ops::getExponent(*it, var) == 1)
	  return it - start;
  return getGeneratorCount();
}

size_t RSFIdeal::getNonMultiple(size_t var) const {
  ASSERT(var < getVarCount());

  const const_iterator stop = end();
  const const_iterator start = begin();
  for (const_iterator it = start; it != stop; ++it)
	if (Ops::getExponent(*it, var) == 0)
	  return it - start;
  return getGeneratorCount();
}

void RSFIdeal::getGcdOfMultiples(Word* gcd, size_t var) const {
  ASSERT(var < getVarCount());

  const Word* const gcdEnd = gcd + getWordsPerTerm();
  Ops::setToAllVarProd(gcd, getVarCount());

  const const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
	if (Ops::getExponent(*it, var) == 1)
	  Ops::gcdInPlace(gcd, gcdEnd, *it);  
}

void RSFIdeal::removeGenerator(size_t gen) {
  Word* term = getGenerator(gen);
  Word* last = getGenerator(getGeneratorCount() - 1);
  if (term != last)
	Ops::assign(term, term + getWordsPerTerm(), last);
  --_genCount;
}

void RSFIdeal::insertNonMultiples(const Word* term,
								  const RawSquareFreeIdeal& ideal) {
  ASSERT(getVarCount() == ideal.getVarCount());

  const Word* termEnd = term + getWordsPerTerm();
  const_iterator stop = ideal.end();
  for (const_iterator it = ideal.begin(); it != stop; ++it)
	if (!Ops::divides(term, termEnd, *it))
	  insert(*it);
}

void RSFIdeal::insertNonMultiples(size_t var,
								  const RawSquareFreeIdeal& ideal) {
  ASSERT(getVarCount() == ideal.getVarCount());

  const_iterator stop = ideal.end();
  for (const_iterator it = ideal.begin(); it != stop; ++it)
	if (Ops::getExponent(*it, var) == 0)
	  insert(*it);
}

size_t RSFIdeal::getNotRelativelyPrime(const Word* term) {
  const size_t wordCount = getWordsPerTerm();
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
	if (!Ops::isRelativelyPrime(term, term + wordCount, getGenerator(gen)))
	  return gen;
  return getGeneratorCount();
}

size_t RSFIdeal::getExclusiveVarGenerator() {
  const size_t wordCount = getWordsPerTerm();
  for (size_t offset = 0; offset < wordCount; ++offset) {
	Word once = 0;
	Word twice = 0;
	for (size_t gen = 0; gen < _genCount; ++gen) {
	  Word word = getGenerator(gen)[offset];
	  twice |= once & word;
	  once |= word;
	}
	const Word onceOnly = once & ~twice;
	if (onceOnly != 0) {
	  for (size_t gen = 0; ; ++gen) {
		ASSERT(gen < _genCount);
		Word word = getGenerator(gen)[offset];
		if (word & onceOnly)
		  return gen;
	  }
	  ASSERT(false);
	}
  }
  return getGeneratorCount();
}

bool RSFIdeal::hasFullSupport(const Word* ignore) const {
  const size_t wordCount = getWordsPerTerm();
  const const_iterator start = begin();
  const const_iterator stop = end();

  for (size_t offset = 0; offset < wordCount; ++offset) {
	Word support = ignore[offset];
	for (const_iterator it = start; it != stop; ++it)
	  support |= (*it)[offset];
	const Word allOnes = ~((Word)0);
	if (support != allOnes) {
	  if (offset == wordCount - 1) {
		const size_t varsLeft = getVarCount() - offset * BitsPerWord;
		ASSERT(varsLeft <= BitsPerWord);
		return Ops::hasFullSupport(&support, varsLeft);
	  } else
		return false;
	}
  }
  return true;
}

bool RSFIdeal::isMinimallyGenerated() const {
  size_t wordCount = getWordsPerTerm();
  for (size_t i = 0; i < _genCount; ++i)
	for (size_t div = 0; div < _genCount; ++div)
	  if (div != i &&
		  Ops::divides(getGenerator(div), getGenerator(div) + wordCount,
					   getGenerator(i)))
		return false;
  return true;
}

void RSFIdeal::swap(size_t a, size_t b) {
  Ops::swap(getGenerator(a), getGenerator(b), getVarCount());
}

bool RSFIdeal::operator==(const RawSquareFreeIdeal& ideal) const {
  if (getVarCount() != ideal.getVarCount())
    return false;
  if (getGeneratorCount() != ideal.getGeneratorCount())
    return false;

  const size_t varCount = getVarCount();
  const_iterator stop = end();  
  const_iterator it = begin();
  const_iterator it2 = ideal.begin();
  for (; it != stop; ++it, ++it2)
    if (!Ops::equals(*it, *it2, varCount))
      return false;
  return true;
}

namespace {
  struct CmpForSortLexAscending : std::binary_function<size_t, size_t, bool> {
	bool operator()(size_t a, size_t b) const {
	  return Ops::lexLess(ideal->getGenerator(a), ideal->getGenerator(b),
						  ideal->getVarCount());
	}
	RawSquareFreeIdeal* ideal;
  };
}

void RSFIdeal::sortLexAscending() {
  vector<size_t> sorted(getGeneratorCount());
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
	sorted[gen] = gen; 
  {
	CmpForSortLexAscending cmp;
	cmp.ideal = this;
	std::sort(sorted.begin(), sorted.end(), cmp);
  }

  RawSquareFreeIdeal* clone =
	newRawSquareFreeIdeal(getVarCount(), getGeneratorCount());
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
	clone->insert(getGenerator(gen));
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
	Ops::assign(getGenerator(gen), clone->getGenerator(sorted[gen]),
				getVarCount());
  deleteRawSquareFreeIdeal(clone);
}

void RSFIdeal::insert(const Word* term) {
  ++_genCount;
  Word* pos = getGenerator(_genCount - 1);
  Ops::assign(pos, pos + getWordsPerTerm(), term);
}

void RSFIdeal::colonReminimize(const Word* by) {
  ASSERT(by != 0);
  colon(by);
  minimize();
}

void RSFIdeal::colonReminimize(size_t var) {
  ASSERT(var < getVarCount());
  const size_t varCount = getVarCount();
  const iterator start = begin();
  iterator stop = end();

  iterator left = start;
  iterator right = stop;
  if (left == right)
	return;
  --right;
  
  while (left != right) {
	while (Ops::getExponent(*left, var) == 1) {
	  ++left;
	  if (left == right)
		goto leftEqRight;
	}
	while (Ops::getExponent(*right, var) == 0) {
	  --right;
	  if (left == right)
		goto leftEqRight;
	}
	Ops::swap(*left, *right, varCount);
  }
 leftEqRight:
  ASSERT(left == right);
  const iterator middle = left;
  
  if (Ops::getExponent(*middle, var) == 1)
	return; // var divides all generators
  if (middle == start)
	return; // var divides no generators

  // var divides [start, middle) and does not divide [middle, end).

  // Do the colon
  for (iterator it = start; it != middle; ++it)
	Ops::setExponent(*it, var, 0);
 
  size_t wordCount = getWordsPerTerm();
  for (iterator it = middle; it != stop;) {
	for (const_iterator div = start; div != middle; ++div) {
	  if (Ops::divides(*div, *div + wordCount, *it) && div != it) {
		--stop;
		Ops::assign(*it, *it + wordCount, *stop);
		--_genCount;
		goto next;
	  }
	}
	++it;
  next:;
  }
}

void RSFIdeal::getLcm(Word* lcm) const {
  Word* lcmEnd = lcm + getWordsPerTerm();
  Ops::setToIdentity(lcm, lcmEnd);
  const const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
	Ops::lcm(lcm, lcmEnd, lcm, *it);
}

RSFIdeal* newRawSquareFreeIdeal(size_t varCount, size_t capacity) {
  size_t byteCount = RSFIdeal::getBytesOfMemoryFor(varCount, capacity);
  void* buffer = new char[byteCount];
  return RSFIdeal::construct(buffer, varCount);
}

RawSquareFreeIdeal* newRawSquareFreeIdealParse(const char* str) {
  istringstream in(str);
  vector<string> lines;
  string line;

  while (getline(in, line))
	lines.push_back(line);

  const size_t varCount = lines.empty() ? 0 : lines.front().size();
  RawSquareFreeIdeal* ideal = newRawSquareFreeIdeal(varCount, lines.size());
  for (size_t gen = 0; gen < lines.size(); ++gen) {
	ASSERT(lines[gen].size() == varCount);
	Word* term = Ops::newTermParse(lines[gen].c_str());
	ideal->insert(term);
	Ops::deleteTerm(term);
  }
  return ideal;
}

void deleteRawSquareFreeIdeal(RSFIdeal* ideal) {
  delete[] reinterpret_cast<char*>(ideal);
}
