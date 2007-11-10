#include "stdinc.h"
#include "Ideal.h"

#include "Term.h"

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

bool Ideal::isIrreducible() const {
  size_t varCount = getVarCount();
  const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
    if (getSizeOfSupport(*it, varCount) != 1)
      return false;
  return true;
}

bool Ideal::isIncomparable(const Term& term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (term.dominates(*it) || term.divides(*it))
      return false;
  return true;
}

bool Ideal::isZeroIdeal() const {
  return _terms.empty();
}

void Ideal::getLcm(Term& lcm) const {
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
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    gcd.gcd(gcd, *it);
}

bool Ideal::contains(const Exponent* term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (::dominates(term, *it, _varCount))
      return true;
  return false;
}






const int ExponentsPerChunk = 100;
const int MinTermsPerChunk = 2;

class ChunkPool {
public:
  Exponent* allocate() {
    if (_chunks.empty()) {
      Exponent* e = new Exponent[ExponentsPerChunk];
      cerr << "allocating: " << e << endl;
      return e;
    }

    Exponent* chunk = _chunks.back();
    cerr << "realloc " << chunk << endl;
    _chunks.pop_back();
    return chunk;
  }

  void deallocate(Exponent* chunk) {
    cerr << "dealloc " << chunk << endl;
    _chunks.push_back(chunk);
  }
  
  ~ChunkPool() {
    for (size_t i = 0; i < _chunks.size(); ++i) {
      cerr << "deleting " << _chunks[i] << endl;
      delete[] _chunks[i];
    }
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
    cerr << "_chunk alloc " << _chunk << endl;
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
    
    for (size_t i = 0; i < _chunks.size(); ++i) {
      cerr << "_chunks[ " << i << "] dealloc " << _chunks[i] << endl;
      globalChunkPool.deallocate(_chunks[i]);
      _chunks[i] = (Exponent*)42;
    }
    _chunks.clear();
    cerr << "size: " << _chunks.size() << endl;
  }

  _varCount = newVarCount;
}

bool Ideal::ExponentAllocator::useSingleChunking() const {
  return _varCount > ExponentsPerChunk / MinTermsPerChunk;
}

// ------------------------------------------------***************

#include <algorithm>
#include <functional>


bool Ideal::filter(FilterFunction& function) {
  bool removedAny = false;

  for (size_t i = 0; i < _terms.size();) {
    if (function(_terms[i])) {
      ++i;
      continue;
    }

    swap(_terms[i], _terms.back());
    _terms.erase(_terms.end() - 1);

    removedAny = true;
  }
  return removedAny;
}

void Ideal::insert(const Term& term) {
  ASSERT(term.getVarCount() == _varCount);
  insert(term.begin());
}

void Ideal::insert(const Ideal& ideal) {
  Ideal::const_iterator stop = ideal.end();
  for (Ideal::const_iterator it = ideal.begin(); it != stop; ++it)
    insert(*it);
}

void Ideal::insert(const Exponent* exponents) {
  Exponent* term = _allocator.allocate();
  copy(exponents, exponents + _varCount, term);
  _terms.push_back(term);
}

void Ideal::singleDegreeSort(size_t variable) {
  std::sort(_terms.begin(), _terms.end(),
	    Term::AscendingSingleDegreeComparator(variable, _varCount));
}

Ideal* Ideal::createMinimizedColon(const Term& by) const {
  Ideal* colon = clone();
  colon->colonReminimize(by);
  return colon;
}

Ideal* Ideal::clone() const {
  return new Ideal(*this);
}

Ideal* Ideal::createNew(size_t varCount) const {
  return new Ideal(varCount);
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

void Ideal::clear() {
  _terms.clear();
  _allocator.reset(_varCount);
}

void Ideal::colon(const Term& by) {
  const_iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it)
    ::colon(*it, *it, by.begin(), _varCount);
}

bool Ideal::removeStrictMultiples(const Exponent* termParam) {
  bool removedAny = false;
  
  for (size_t i = 0; i < _terms.size();) {
    if (!::strictlyDivides(termParam, _terms[i], _varCount)) {
      ++i;
      continue;
    }
    
    swap(_terms[i], _terms.back());
    _terms.erase(_terms.end() - 1);
    
    removedAny = true;
  }
  return removedAny;
}

void Ideal::print() const {
  cerr << "//------------ Ideal:\n";
  for (const_iterator it = _terms.begin(); it != _terms.end(); ++it) {
    Term term(*it, _varCount);
    cerr << term << '\n';
  }
  cerr << "------------\\\\" << endl;
}
//*
void Ideal::minimize() {
  if (_terms.empty())
    return;

  std::sort(_terms.begin(), _terms.end(),
	    Term::LexComparator(_varCount));

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
//*/

/*
void Ideal::minimize() {
  for (size_t i = 0; i < _terms.size();) {
    bool remove = false;
    
    size_t idealSize = _terms.size();
    for (size_t j = 0; j < idealSize; ++j) {
      if (i == j)
	continue;
      
      if (::divides(_terms[j], _terms[i], _varCount)) {
	remove = true;
	break;
      }
    }
    
    if (remove) {
      swap(_terms[i], _terms.back());
      _terms.erase(_terms.end() - 1);
    } else
      ++i;
  }
}
//*/

// Making it into an object ensures proper destruction and hence
// deallocation. This prevents spurious reports from memory leak
// detectors.
class IdealCache {
public:
  ~IdealCache() {
    for (size_t i = 0; i < _cache.size(); ++i)
      free(_cache[i]);
  }

  void* allocate(size_t size) {
    ASSERT(size == sizeof(Ideal));

    if (_cache.empty())
      return malloc(size);

    void* p = _cache.back();
    _cache.pop_back();
    return p;
  }

  void deallocate(void* p, size_t size) {
    ASSERT(size == sizeof(Ideal));

    _cache.push_back(p);
  }

  private:
    vector<void*> _cache;
} termListCache;
/*
void* Ideal::operator new(size_t size) {
  return termListCache.allocate(size);
}

void Ideal::operator delete(void* p, size_t size) {
  termListCache.deallocate(p, size);
}
*/
void Ideal::removeDuplicates() {
  std::sort(_terms.begin(), _terms.end(), Term::LexComparator(_varCount));
  iterator newEnd =
    unique(_terms.begin(), _terms.end(), Term::EqualsPredicate(_varCount));
  _terms.erase(newEnd, _terms.end());
}
