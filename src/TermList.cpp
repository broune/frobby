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
  _varCount(ideal.getVariableCount()),
  _allocator(ideal.getVariableCount()) {

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
	    Term::SingleDegreeComparator(variable, _varCount));
}

bool TermList::isIncomparable(const Term& term) const {
  const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
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
  const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
    lcm.lcm(lcm, *it);
}

void TermList::getGcd(Term& gcd) const {
  if (_terms.empty()) {
    gcd.setToIdentity();
    return;
  }

  gcd = _terms[0];
  const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
    gcd.gcd(gcd, *it);
}

bool TermList::contains(const Term& term) const {
  const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
    if (term.dominates(*it))
      return true;
  return false;
}

Ideal* TermList::createMinimizedColon(const Term& by) const {
  Ideal* colon = clone();
  colon->colon(by);
  colon->minimize();

  return colon;
}

Ideal* TermList::clone() const {
  return new TermList(*this);
}

void TermList::clear() {
  _terms.clear();
  _allocator.clear();
}

void TermList::colon(const Term& by) {
  const_iterator stop = end();
  for (iterator it = _terms.begin(); it != stop; ++it)
    ::colon(*it, *it, by.begin(), _varCount);
}

bool TermList::removeStrictMultiples(const Term& termParam) {
  bool removedAny = false;
  
  for (size_t i = 0; i < _terms.size();) {
    if (!termParam.strictlyDivides(_terms[i])) {
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
  for (const_iterator it = begin(); it != end(); ++it) {
    Term term(*it, _varCount);
    cerr << term << '\n';
  }
  cerr << "------------\\\\" << endl;
}

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


/*
#include "TermTree.h"
void TermList::minimize() {
  TermTree tree(getVariableCount());
  
  Term term(getVariableCount());

  const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it) {
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
