#include "stdinc.h"
#include "TermList.h"
/*
#include <algorithm>
#include <functional>

TermList::TermList(unsigned int varCount):
  Ideal(varCount) {
}

TermList::TermList(const Ideal& ideal):
  Ideal(ideal.getVarCount()) {

  _terms.reserve(ideal.getGeneratorCount());
  insert(ideal);
}

TermList::TermList(const TermList& ideal):
  Ideal(ideal.getVarCount()) {

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

void TermList::singleDegreeSort(size_t variable) {
  std::sort(_terms.begin(), _terms.end(),
	    Term::AscendingSingleDegreeComparator(variable, _varCount));
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

// Making it into an object ensures proper destruction and hence
// deallocation. This prevents spurious reports from memory leak
// detectors.
class TermListCache {
public:
  ~TermListCache() {
    for (size_t i = 0; i < _cache.size(); ++i)
      free(_cache[i]);
  }

  void* allocate(size_t size) {
    ASSERT(size == sizeof(TermList));

    if (_cache.empty())
      return malloc(size);

    void* p = _cache.back();
    _cache.pop_back();
    return p;
  }

  void deallocate(void* p, size_t size) {
    ASSERT(size == sizeof(TermList));

    _cache.push_back(p);
  }

  private:
    vector<void*> _cache;
} termListCache;

void* TermList::operator new(size_t size) {
  return termListCache.allocate(size);
}

void TermList::operator delete(void* p, size_t size) {
  termListCache.deallocate(p, size);
}

void TermList::removeDuplicates() {
  std::sort(_terms.begin(), _terms.end(), Term::LexComparator(_varCount));
  iterator newEnd =
    unique(_terms.begin(), _terms.end(), Term::EqualsPredicate(_varCount));
  _terms.erase(newEnd, _terms.end());
}
*/
