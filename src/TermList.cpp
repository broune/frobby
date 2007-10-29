#include "stdinc.h"
#include "TermList.h"

#include <algorithm>
#include <functional>

TermList::TermList(unsigned int varCount):
  _varCount(varCount),
  _allocator(varCount) {
}

TermList::TermList(const Ideal& ideal):
  _varCount(ideal.getVariableCount()),
  _allocator(ideal.getVariableCount()) {

  _terms.reserve(ideal.getGeneratorCount());
  insert(ideal);
}

TermList::TermList(const TermList& ideal):
  Ideal(),
  _varCount(ideal._varCount),
  _allocator(ideal._varCount) {

  _terms.reserve(ideal.getGeneratorCount());
  insert(ideal);
}

bool TermList::filter(FilterFunction& function) {
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

void TermList::insert(const Term& term) {
  ASSERT(term.getVarCount() == _varCount);
  insert(term.begin());
}

void TermList::insert(const Ideal& ideal) {
  Ideal::const_iterator stop = ideal.end();
  for (Ideal::const_iterator it = ideal.begin(); it != stop; ++it)
    insert(*it);
}

void TermList::insert(const Exponent* exponents) {
  Exponent* term = _allocator.allocate();
  copy(exponents, exponents + _varCount, term);
  _terms.push_back(term);
}

TermList::const_iterator TermList::begin() const {
  return _terms.begin();
}

TermList::const_iterator TermList::end() const {
  return _terms.end();
}

void TermList::singleDegreeSort(size_t variable) {
  std::sort(_terms.begin(), _terms.end(),
	    Term::AscendingSingleDegreeComparator(variable, _varCount));
}

bool TermList::isIncomparable(const Term& term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (term.dominates(*it) || term.divides(*it))
      return false;
  return true;
}

size_t TermList::size() const {
  return _terms.size();
}

size_t TermList::getVariableCount() const {
  return _varCount;
}

size_t TermList::getGeneratorCount() const {
  return _terms.size();
}

bool TermList::isZeroIdeal() const {
  return _terms.empty();
}

void TermList::getLcm(Term& lcm) const {
  lcm.setToIdentity();
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    lcm.lcm(lcm, *it);
}

void TermList::getGcd(Term& gcd) const {
  if (_terms.empty()) {
    gcd.setToIdentity();
    return;
  }

  gcd = _terms[0];
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    gcd.gcd(gcd, *it);
}

bool TermList::contains(const Term& term) const {
  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it)
    if (term.dominates(*it))
      return true;
  return false;
}

Ideal* TermList::createMinimizedColon(const Term& by) const {
  TermList* colon = new TermList(*this);
  colon->colonReminimize(by);
  return colon;
}

void TermList::colonReminimize(const Term& by) {
  if (by.getSizeOfSupport() == 1) {
    size_t var = by.getFirstNonZeroExponent();
    colonReminimize(var, by[var]);
  } else {
    colon(by);
    minimize();
  }
}

void TermList::colonReminimize(size_t var, Exponent exp) {
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

Ideal* TermList::clone() const {
  return new TermList(*this);
}

Ideal* TermList::createNew(size_t varCount) {
  return new TermList(varCount);
}

void TermList::clear() {
  _terms.clear();
  _allocator.reset(_varCount);
}

void TermList::colon(const Term& by) {
  const_iterator stop = _terms.end();
  for (iterator it = _terms.begin(); it != stop; ++it)
    ::colon(*it, *it, by.begin(), _varCount);
}

bool TermList::removeStrictMultiples(const Exponent* termParam) {
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

void TermList::print() const {
  cerr << "//------------ TermList:\n";
  for (const_iterator it = _terms.begin(); it != _terms.end(); ++it) {
    Term term(*it, _varCount);
    cerr << term << '\n';
  }
  cerr << "------------\\\\" << endl;
}

void TermList::minimize() {
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


/*
void TermList::minimize() {
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
*/

/*
#include "TermTree.h"
void TermList::minimize() {
  TermTree tree(getVariableCount());
  
  Term term(getVariableCount());

  const_iterator stop = _terms.end();
  for (const_iterator it = _terms.begin(); it != stop; ++it) {
    term = *it;
    if (!tree.getDivisor(term)) {
      tree.removeDominators(term);
      tree.insert(term);
    }
  }
  
  clear();
  tree.getTerms(*this);
}
*/

const int ExponentsPerChunk = 100;
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

private:
  vector<Exponent*> _chunks;
} globalChunkPool;

TermList::ExponentAllocator::ExponentAllocator(size_t varCount):
  _varCount(varCount),
  _chunk(0),
  _chunkIterator(0),
  _chunkEnd(0) {
}

TermList::ExponentAllocator::~ExponentAllocator() {
  reset();
}

Exponent* TermList::ExponentAllocator::allocate() {
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

void TermList::ExponentAllocator::reset(size_t newVarCount) {
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

bool TermList::ExponentAllocator::useSingleChunking() const {
  return _varCount > ExponentsPerChunk / MinTermsPerChunk;
}
