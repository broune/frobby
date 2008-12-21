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
#include "stdinc.h"
#include "Ideal.h"

#include "Term.h"
#include "Minimizer.h"
#include <algorithm>
#include <functional>

Ideal::~Ideal() {
}

Ideal::Ideal(size_t varCount):
  _varCount(varCount),
  _allocator(varCount) {
}

Ideal::Ideal(const Ideal& ideal):
  _varCount(ideal.getVarCount()),
  _allocator(ideal.getVarCount()) {
  insert(ideal);
}

bool Ideal::isIncomparable(const Exponent* term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (::dominates(term, *it, _varCount) ||
		::divides(term, *it, _varCount))
      return false;
  return true;
}

bool Ideal::contains(const Exponent* term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (::dominates(term, *it, _varCount))
      return true;
  return false;
}

bool Ideal::containsIdentity() const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (::isIdentity(*it, _varCount))
      return true;
  return false;
}

bool Ideal::strictlyContains(const Exponent* term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (::strictlyDivides(*it, term, _varCount))
      return true;
  return false;
}

bool Ideal::isMinimallyGenerated() const {
  Minimizer minimizer(_varCount);
  return minimizer.isMinimallyGenerated(_terms.begin(), _terms.end());
}

bool Ideal::isZeroIdeal() const {
  return _terms.empty();
}

bool Ideal::isIrreducible() const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (getSizeOfSupport(*it, _varCount) != 1)
      return false;
  return true;
}

bool Ideal::isStronglyGeneric() {
  for (size_t var = 0; var < _varCount; ++var) {
	singleDegreeSort(var);

	Exponent lastExponent = 0;
	const_iterator stop = _terms.end();
	for (const_iterator it = _terms.begin(); it != stop; ++it) {
	  if (lastExponent != 0 && lastExponent == (*it)[var])
		return false;
	  lastExponent = (*it)[var];
	}
  }
  return true;
}

void Ideal::getLcm(Exponent* lcm) const {
  ::setToIdentity(lcm, _varCount);
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
	::lcm(lcm, lcm, *it, _varCount);
}

void Ideal::getGcd(Exponent* gcd) const {
  if (_terms.empty()) {
	::setToIdentity(gcd, _varCount);
    return;
  }

  copy(_terms[0], _terms[0] + _varCount, gcd);
  const_iterator stop = _terms.end();
  const_iterator it = _terms.begin();
  for (++it; it != stop; ++it)
    ::gcd(gcd, gcd, *it, _varCount);
}

void Ideal::getLeastExponents(Exponent* least) const {
  ::setToIdentity(least, _varCount);
  
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
	for (size_t var = 0; var < _varCount; ++var)
	  if (least[var] == 0 || ((*it)[var] < least[var] && (*it)[var] > 0))
		least[var] = (*it)[var];
}

void Ideal::getSupportCounts(Exponent* counts) const {
  ::setToIdentity(counts, _varCount);
  const_iterator stop = _terms.end();
  for (const_iterator it = begin(); it != stop; ++it)
	for (size_t var = 0; var < _varCount; ++var)
	  if ((*it)[var] > 0)
		counts[var] += 1;
}

bool Ideal::operator==(const Ideal& ideal) const {
  if (getVarCount() != ideal.getVarCount())
	return false;
  if (getGeneratorCount() != ideal.getGeneratorCount())
	return false;

  const_iterator stop = _terms.end();
  const_iterator it = begin();
  const_iterator it2 = ideal.begin();
  for (; it != stop; ++it, ++it2)
	if (!equals(*it, *it2, getVarCount()))
	  return false;

  return true;
}

void Ideal::print(FILE* file) const {
  fputs("//------------ Ideal:\n", file);
  for (const_iterator it = _terms.begin(); it != _terms.end(); ++it) {
    ::print(file, *it, _varCount);
    fputc('\n', file);
  }
  fputs("------------\\\\\n", file);
}

void Ideal::insert(const Exponent* exponents) {
  Exponent* term = _allocator.allocate();
  copy(exponents, exponents + _varCount, term);

  // push_back could throw bad_alloc, but the allocator is already
  // keeping track of the allocated memory, so there is not a memory
  // leak.
  _terms.push_back(term);
}

void Ideal::insert(const Ideal& ideal) {
  _terms.reserve(_terms.size() + ideal._terms.size());
  Ideal::const_iterator stop = ideal.end();
  for (Ideal::const_iterator it = ideal.begin(); it != stop; ++it)
    insert(*it);
}

void Ideal::insertReminimize(const Exponent* term) {
  ASSERT(isMinimallyGenerated());
  if (contains(term))
	return;

  removeMultiples(term);
  insert(term);
  ASSERT(isMinimallyGenerated());
}

void Ideal::minimize() {
  if (_terms.empty())
    return;

  Minimizer minimizer(_varCount);
  _terms.erase(minimizer.minimize(_terms.begin(), _terms.end()), _terms.end());
  ASSERT(isMinimallyGenerated());
}

void Ideal::sortReverseLex() {
  std::sort(_terms.begin(), _terms.end(),
			Term::ReverseLexComparator(_varCount));
}

void Ideal::sortLex() {
  std::sort(_terms.begin(), _terms.end(), Term::LexComparator(_varCount));
}

void Ideal::singleDegreeSort(size_t var) {
  ASSERT(var < _varCount);
  std::sort(_terms.begin(), _terms.end(),
	    Term::AscendingSingleDegreeComparator(var, _varCount));
}

void Ideal::product(const Exponent* by) {
  iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it)
    ::product(*it, *it, by, _varCount);
}

void Ideal::colon(const Exponent* by) {
  iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it)
    ::colon(*it, *it, by, _varCount);
}

void Ideal::colon(size_t var, Exponent e) {
  iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it) {
	Exponent& ite = (*it)[var];
	if (ite != 0) {
	  if (ite > e)
		ite -= e;
	  else
		ite = 0;
	}
  }
}

bool Ideal::colonReminimize(const Exponent* by) {
  ASSERT(isMinimallyGenerated());

  Minimizer minimizer(_varCount);
  pair<iterator, bool> pair =
	minimizer.colonReminimize(_terms.begin(), _terms.end(), by);

  _terms.erase(pair.first, _terms.end());

  ASSERT(isMinimallyGenerated());
  return pair.second;
}

bool Ideal::colonReminimize(size_t var, Exponent e) {
  ASSERT(isMinimallyGenerated());

  Minimizer minimizer(_varCount);
  pair<iterator, bool> pair =
	minimizer.colonReminimize(_terms.begin(), _terms.end(), var, e);

  _terms.erase(pair.first, _terms.end());

  ASSERT(isMinimallyGenerated());
  return pair.second;
}

void Ideal::remove(const_iterator it) {
  ASSERT(begin() <= it);
  ASSERT(it < end());
  ::swap(const_cast<Exponent*&>(*it), *(_terms.end() - 1));
  _terms.pop_back();
}


void Ideal::removeMultiples(const Exponent* term) {
  iterator newEnd = _terms.begin();
  iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it) {
    if (!::divides(term, *it, _varCount)) {
	  *newEnd = *it;
	  ++newEnd;
    }
  }
  _terms.erase(newEnd, stop);
}

void Ideal::removeMultiples(size_t var, Exponent e) {
  iterator newEnd = _terms.begin();
  iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it) {
    if ((*it)[var] < e) {
	  *newEnd = *it;
	  ++newEnd;
    }
  }
  _terms.erase(newEnd, stop);
}

void Ideal::insertNonMultiples(const Exponent* term, const Ideal& ideal) {
  const_iterator stop = ideal.end();
  for (const_iterator it = ideal.begin(); it != stop; ++it)
    if (!::divides(term, *it, _varCount))
	  insert(*it);
}

void Ideal::insertNonMultiples(size_t var, Exponent e, const Ideal& ideal) {
  const_iterator stop = ideal.end();
  for (const_iterator it = ideal.begin(); it != stop; ++it)
    if ((*it)[var] < e)
	  insert(*it);
}

void Ideal::removeStrictMultiples(const Exponent* term) {
  iterator newEnd = _terms.begin();
  iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it) {
    if (!::strictlyDivides(term, *it, _varCount)) {
	  *newEnd = *it;
	  ++newEnd;
    }
  }
  _terms.erase(newEnd, stop);
}

void Ideal::removeDuplicates() {
  std::sort(_terms.begin(), _terms.end(), Term::LexComparator(_varCount));
  iterator newEnd =
    unique(_terms.begin(), _terms.end(), Term::EqualsPredicate(_varCount));
  _terms.erase(newEnd, _terms.end());
}

void Ideal::clear() {
  _terms.clear();
  _allocator.reset(_varCount);
}

void Ideal::clearAndSetVarCount(size_t varCount) {
  _varCount = varCount;
  _terms.clear();
  _allocator.reset(varCount);
}

Ideal::const_iterator Ideal::getMultiple(size_t var) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
	if ((*it)[var] > 0)
	  return it;
  return stop;
}

Ideal& Ideal::operator=(const Ideal& ideal) {
  clearAndSetVarCount(ideal.getVarCount());
  insert(ideal);

  return *this;
}

void Ideal::swap(Ideal& ideal) {
  std::swap(_varCount, ideal._varCount);
  _terms.swap(ideal._terms);
  _allocator.swap(ideal._allocator);
}




// ---------------------***************







const int ExponentsPerChunk = 1024;
const int MinTermsPerChunk = 2;

class ChunkPool {
public:
  Exponent* allocate() {
    if (_chunks.empty())
      return new Exponent[ExponentsPerChunk];

    Exponent* chunk = _chunks.back();
    _chunks.pop_back();
    return chunk;
  }

  void deallocate(Exponent* chunk) {
	// deallocate can be called from a destructor, so no exceptions
	// can be allowed to escape from it.
	try {
	  _chunks.push_back(chunk);
	} catch (const bad_alloc&) {
	  delete[] chunk;
	}
  }

  void clear() {
    for (size_t i = 0; i < _chunks.size(); ++i)
      delete[] _chunks[i];
	_chunks.clear();
  }

  ~ChunkPool() {
	clear();
  }

private:
  vector<Exponent*> _chunks;
} globalChunkPool;

Ideal::ExponentAllocator::ExponentAllocator(size_t varCount):
  _varCount(varCount),
  _chunkIterator(0),
  _chunkEnd(0) {
  if (_varCount == 0)
	_varCount = 1;
}

Ideal::ExponentAllocator::~ExponentAllocator() {
  reset(0);
}

Exponent* Ideal::ExponentAllocator::allocate() {
  if (_chunkIterator + _varCount > _chunkEnd) {
    if (useSingleChunking()) {
      Exponent* term = new Exponent[_varCount];
	  try {
		_chunks.push_back(term);
	  } catch (...) {
		delete[] term;
		throw;
	  }
      return term;
    }

	_chunkIterator = globalChunkPool.allocate();
	_chunkEnd = _chunkIterator + ExponentsPerChunk;
	
	try {
	  _chunks.push_back(_chunkIterator);
	} catch (...) {
	  globalChunkPool.deallocate(_chunkIterator);
	  throw;
	}
  }

  Exponent* term = _chunkIterator;
  _chunkIterator += _varCount;
  ASSERT(_chunkIterator <= _chunkEnd);

  return term;
}

void Ideal::ExponentAllocator::reset(size_t newVarCount) {
  _varCount = newVarCount;

  if (useSingleChunking()) {
    for (size_t i = 0; i < _chunks.size(); ++i)
      delete[] _chunks[i];
    _chunks.clear();
  } else {
    _chunkIterator = 0;
    _chunkEnd = 0;
    
    for (size_t i = 0; i < _chunks.size(); ++i)
      globalChunkPool.deallocate(_chunks[i]);
    _chunks.clear();
  }
}

void Ideal::ExponentAllocator::swap(ExponentAllocator& allocator) {
  std::swap(_varCount, allocator._varCount);
  std::swap(_chunkIterator, allocator._chunkIterator);
  std::swap(_chunkEnd, allocator._chunkEnd);

  _chunks.swap(allocator._chunks);
}

bool Ideal::ExponentAllocator::useSingleChunking() const {
  return _varCount > ExponentsPerChunk / MinTermsPerChunk;
}

void Ideal::clearStaticCache() {
  globalChunkPool.clear();
}
