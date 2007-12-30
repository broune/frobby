#include "stdinc.h"
#include "Ideal.h"

#include "Term.h"
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

bool Ideal::isIncomparable(const Term& term) const {
  ASSERT(term.getVarCount() == getVarCount());
  return isIncomparable(term.begin());
}

bool Ideal::contains(const Exponent* term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (::dominates(term, *it, _varCount))
      return true;
  return false;
}

bool Ideal::contains(const Term& term) const {
  ASSERT(term.getVarCount() == getVarCount());
  return contains(term.begin());
}

bool Ideal::strictlyContains(const Term& term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (::strictlyDivides(*it, term, _varCount))
      return true;
  return false;
}

bool Ideal::isIrreducible() const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (getSizeOfSupport(*it, _varCount) != 1)
      return false;
  return true;
}

void Ideal::getLcm(Term& lcm) const {
  ASSERT(lcm.getVarCount() == getVarCount());

  lcm.setToIdentity();
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    lcm.lcm(lcm, *it);
}

void Ideal::getGcd(Term& gcd) const {
  if (_terms.empty()) {
    gcd.setToIdentity();
    return;
  }

  gcd = _terms[0];
  const_iterator stop = _terms.end();
  const_iterator it = _terms.begin();
  ++it;
  for (; it != stop; ++it)
    gcd.gcd(gcd, *it);
}

void Ideal::print(FILE* file) const {
  fputs("//------------ Ideal:\n", file);
  for (const_iterator it = _terms.begin(); it != _terms.end(); ++it) {
    ::print(file, *it, _varCount);
    fputc('\n', file);
  }
  fputs("------------\\\\", file);
}

void Ideal::insert(const Term& term) {
  ASSERT(term.getVarCount() == _varCount);
  insert(term.begin());
}

void Ideal::insert(const Exponent* exponents) {
  Exponent* term = _allocator.allocate();
  copy(exponents, exponents + _varCount, term);
  _terms.push_back(term);
}

void Ideal::insert(const Ideal& ideal) {
  _terms.reserve(_terms.size() + ideal._terms.size());
  Ideal::const_iterator stop = ideal.end();
  for (Ideal::const_iterator it = ideal.begin(); it != stop; ++it)
    insert(*it);
}

void Ideal::minimize() {
  if (_terms.empty())
    return;

  std::sort(_terms.begin(), _terms.end(), Term::LexComparator(_varCount));

  size_t newEnd = 0;
  for (size_t i = 0; i < _terms.size(); ++i) {
    bool remove = false;

    for (size_t j = 0; j < newEnd; ++j) {
      if (i == j)
	continue;

      if (::divides(_terms[j], _terms[i], _varCount)) {
	remove = true;
	break;
      }
    }

    if (!remove) {
      _terms[newEnd] = _terms[i];
      ++newEnd;
    }
  }

  _terms.erase(_terms.begin() + newEnd, _terms.end());
}

void Ideal::colon(const Term& by) {
  iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it)
    ::colon(*it, *it, by.begin(), _varCount);
}

void Ideal::colonReminimize(const Term& by) {
  if (by.getSizeOfSupport() == 1) {
    size_t var = by.getFirstNonZeroExponent();
    colonReminimize(var, by[var]);
  } else {
    colon(by);
    minimize();
  }
}

void Ideal::colonReminimize(size_t var, Exponent exp) {
  std::sort(_terms.begin(), _terms.end(),
	    Term::DescendingSingleDegreeComparator(var, _varCount));
  size_t idealSize = _terms.size();

  // These can be kept without any further processing at all.
  size_t initialBlockEnd = 0;
  while (initialBlockEnd < idealSize &&
	 _terms[initialBlockEnd][var] > exp) {
    _terms[initialBlockEnd][var] -= exp; // perform colon
    ++initialBlockEnd;
  }

  // We group terms into blocks according to term[var].
  size_t previousBlockEnd = initialBlockEnd;
  size_t newEnd = initialBlockEnd;
  Exponent block = 0; // the initial value here does not actually matter

  // We can start with i = 1 because the first element is always kept.
  for (size_t i = initialBlockEnd; i < idealSize; ++i) {
    // Detect if we are moving on to next block.
    if (_terms[i][var] != block) {
      block = _terms[i][var];
      previousBlockEnd = newEnd;
    }

    // Perform colon
    if (_terms[i][var] > exp)
      _terms[i][var] -= exp;
    else
      _terms[i][var] = 0;

    bool remove = false;

    for (size_t j = initialBlockEnd; j < previousBlockEnd; ++j) {
      if (::divides(_terms[j], _terms[i], _varCount)) {
	remove = true;
	break;
      }
    }
    
    if (!remove) {
      _terms[newEnd] = _terms[i];
      ++newEnd;
    }
  }

  _terms.erase(_terms.begin() + newEnd, _terms.end());
}

void Ideal::removeStrictMultiples(const Exponent* termParam) {
  for (size_t i = 0; i < _terms.size();) {
    if (!::strictlyDivides(termParam, _terms[i], _varCount)) {
      ++i;
      continue;
    }

    std::swap(_terms[i], _terms.back());
    _terms.erase(_terms.end() - 1);
  }
}

void Ideal::removeDuplicates() {
  std::sort(_terms.begin(), _terms.end(), Term::LexComparator(_varCount));
  iterator newEnd =
    unique(_terms.begin(), _terms.end(), Term::EqualsPredicate(_varCount));
  _terms.erase(newEnd, _terms.end());
}

void Ideal::singleDegreeSort(size_t var) {
  ASSERT(var < _varCount);
  std::sort(_terms.begin(), _terms.end(),
	    Term::AscendingSingleDegreeComparator(var, _varCount));
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
}

Ideal::ExponentAllocator::~ExponentAllocator() {
  reset(0);
}

Exponent* Ideal::ExponentAllocator::allocate() {
  ASSERT(_varCount >= 1);

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
