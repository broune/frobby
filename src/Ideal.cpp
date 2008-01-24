#include "stdinc.h"
#include "Ideal.h"

#include "Term.h"
#include "Minimizer.h"
#include <algorithm>
#include <functional>

Ideal::~Ideal() {
}

Ideal::Ideal(unsigned int varCount):
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

bool Ideal::strictlyContains(const Exponent* term) const {
  const_iterator end = _terms.end();
  for (const_iterator it = _terms.begin(); it != end; ++it)
    if (::strictlyDivides(*it, term, _varCount))
      return true;
  return false;
}

bool Ideal::isMinimallyGenerated() const {
  Minimizer minimizer(_varCount);
  return minimizer.isMinimallyGenerated(_terms.begin(), _terms.end());
}

bool Ideal::isIrreducible() const {
  const_iterator end = _terms.end();
  for (const_iterator it = _terms.begin(); it != end; ++it)
    if (getSizeOfSupport(*it, _varCount) != 1)
      return false;
  return true;
}

bool Ideal::isStronglyGeneric() {
  for (size_t var = 0; var < _varCount; ++var) {
	singleDegreeSort(var);

	Exponent lastExponent = 0;
	const_iterator end = _terms.end();
	for (const_iterator it = _terms.begin(); it != end; ++it) {
	  if (lastExponent != 0 && lastExponent == (*it)[var])
		return false;
	  lastExponent = (*it)[var];
	}
  }
  return true;
}

void Ideal::getLcm(Exponent* lcm) const {
  ::setToIdentity(lcm, _varCount);
  const_iterator end = _terms.end();
  for (const_iterator it = _terms.begin(); it != end; ++it)
	::lcm(lcm, lcm, *it, _varCount);
}

void Ideal::getGcd(Exponent* gcd) const {
  if (_terms.empty()) {
	::setToIdentity(gcd, _varCount);
    return;
  }

  copy(_terms[0], _terms[0] + _varCount, gcd);
  const_iterator end = _terms.end();
  const_iterator it = _terms.begin();
  for (++it; it != end; ++it)
    ::gcd(gcd, gcd, *it, _varCount);
}

void Ideal::getSupportCounts(Exponent* counts) const {
  ::setToIdentity(counts, _varCount);
  const_iterator end = _terms.end();
  for (const_iterator it = begin(); it != end; ++it)
	for (size_t var = 0; var < _varCount; ++var)
	  if ((*it)[var] > 0)
		counts[var] += 1;
}

bool Ideal::operator==(const Ideal& ideal) const {
  if (getVarCount() != ideal.getVarCount())
	return false;
  if (getGeneratorCount() != ideal.getGeneratorCount())
	return false;

  const_iterator end = _terms.end();
  const_iterator it = begin();
  const_iterator it2 = ideal.begin();
  for (; it != end; ++it, ++it2)
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
  _terms.push_back(term);
}

void Ideal::insert(const Ideal& ideal) {
  _terms.reserve(_terms.size() + ideal._terms.size());
  Ideal::const_iterator end = ideal.end();
  for (Ideal::const_iterator it = ideal.begin(); it != end; ++it)
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

void Ideal::colon(const Exponent* colon) {
  iterator end = _terms.end();
  for (iterator it = _terms.begin(); it != end; ++it)
    ::colon(*it, *it, colon, _varCount);
}

bool Ideal::colonReminimize(const Exponent* colon) {
  ASSERT(isMinimallyGenerated());

  Minimizer minimizer(_varCount);
  pair<iterator, bool> pair =
	minimizer.colonReminimize(_terms.begin(), _terms.end(), colon);

  _terms.erase(pair.first, _terms.end());

  ASSERT(isMinimallyGenerated());
  return pair.second;
}

void Ideal::removeMultiples(const Exponent* term) {
  iterator newEnd = _terms.begin();
  iterator end = _terms.end();
  for (iterator it = _terms.begin(); it != end; ++it) {
    if (!::divides(term, *it, _varCount)) {
	  *newEnd = *it;
	  ++newEnd;
    }
  }
  _terms.erase(newEnd, end);
}

void Ideal::removeStrictMultiples(const Exponent* term) {
  iterator newEnd = _terms.begin();
  iterator end = _terms.end();
  for (iterator it = _terms.begin(); it != end; ++it) {
    if (!::strictlyDivides(term, *it, _varCount)) {
	  *newEnd = *it;
	  ++newEnd;
    }
  }
  _terms.erase(newEnd, end);
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







const int ExponentsPerChunk = 1000;
const int MinTermsPerChunk = 2;

class ChunkPool {
public:
  Exponent* allocate() {
    if (_chunks.empty()) {
      Exponent* e = new Exponent[ExponentsPerChunk];
      return e;
    }

    Exponent* chunk = _chunks.back();
    _chunks.pop_back();
    return chunk;
  }

  void deallocate(Exponent* chunk) {
    _chunks.push_back(chunk);
  }
  
  ~ChunkPool() {
    for (size_t i = 0; i < _chunks.size(); ++i)
      delete[] _chunks[i];
  }

private:
  vector<Exponent*> _chunks;
} globalChunkPool;

Ideal::ExponentAllocator::ExponentAllocator(size_t varCount):
  _varCount(varCount),
  _chunk(0),
  _chunkIterator(0),
  _chunkEnd(0) {
  if (_varCount == 0)
	_varCount = 1; // Otherwise strange things happen
}

Ideal::ExponentAllocator::~ExponentAllocator() {
  reset(0);
}

Exponent* Ideal::ExponentAllocator::allocate() {
  if (_chunkIterator + _varCount > _chunkEnd) {
    if (useSingleChunking()) {
      Exponent* term = new Exponent[_varCount];
      _chunks.push_back(term);
      return term;
    }

    _chunk = globalChunkPool.allocate();
    _chunkIterator = _chunk;
    _chunkEnd = _chunk + ExponentsPerChunk;

    _chunks.push_back(_chunk);
  }

  Exponent* term = _chunkIterator;
  _chunkIterator += _varCount;
  ASSERT(_chunkIterator <= _chunkEnd);

  return term;
}

void Ideal::ExponentAllocator::reset(size_t newVarCount) {
  if (useSingleChunking()) {
    for (size_t i = 0; i < _chunks.size(); ++i)
      delete[] _chunks[i];
    _chunks.clear();
  } else {
    _chunk = 0;
    _chunkIterator = 0;
    _chunkEnd = 0;
    
    for (size_t i = 0; i < _chunks.size(); ++i)
      globalChunkPool.deallocate(_chunks[i]);
    _chunks.clear();
  }

  _varCount = newVarCount;
}

void Ideal::ExponentAllocator::swap(ExponentAllocator& allocator) {
  std::swap(_varCount, allocator._varCount);
  std::swap(_chunk, allocator._chunk);
  std::swap(_chunkIterator, allocator._chunkIterator);
  std::swap(_chunkEnd, allocator._chunkEnd);

  _chunks.swap(allocator._chunks);
}

bool Ideal::ExponentAllocator::useSingleChunking() const {
  return _varCount > ExponentsPerChunk / MinTermsPerChunk;
}
