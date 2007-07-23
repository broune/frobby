#include "stdinc.h"
#include "TermList.h"

TermList::TermList(unsigned int varCount):
  _varCount(varCount) {
}

void TermList::insert(const Term& term) {
  _terms.push_back(term);
}

TermList::const_iterator TermList::begin() const {
  return _terms.begin();
}

TermList::const_iterator TermList::end() const {
  return _terms.end();
}

bool TermList::isIncomparable(const Term& term) const {
  for (const_iterator it = begin(); it != end(); ++it)
    if (it->divides(term) || term.divides(*it))
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
  lcm.setToZero();
  for (const_iterator it = begin(); it != end(); ++it)
    lcm.lcm(lcm, *it);
}

void TermList::getGcd(Term& gcd) const {
  gcd.setToZero();
  for (const_iterator it = begin(); it != end(); ++it)
    gcd.gcd(gcd, *it);
}

bool TermList::contains(const Term& term) const {
  for (const_iterator it = begin(); it != end(); ++it)
    if (it->divides(term))
      return true;
  return false;
}

void TermList::colonMinimize(const TermList& ideal, const Term& by) {
  _terms.clear();
  _terms.reserve(ideal.getGeneratorCount());

  for (const_iterator it = ideal.begin(); it != ideal.end(); ++it) {
    _terms.push_back(Term(_varCount));
    _terms.back().colon(*it, by);
  }

  minimize();
}

void TermList::colon(const Term& by) {
  for (iterator it = _terms.begin(); it != end(); ++it)
    it->colon(*it, by);
}

void TermList::minimize() {
  for (iterator it1 = _terms.begin(); it1 != end();) {
    bool remove = false;
    for (const_iterator it2 = begin(); it2 != end(); ++it2) {
      if (it1 == it2)
	continue;
      if (it2->divides(*it1)) {
	remove = true;
	break;
      }
    }
    
    if (remove) {
      swap(*it1, _terms.back());
      _terms.erase(_terms.end() - 1);
    } else
      ++it1;
  }
}

void TermList::removeStrictMultiples(const Term& term) {
  for (iterator it1 = _terms.begin(); it1 != end();) {
    if (term.strictlyDivides(*it1)) {
      swap(*it1, _terms.back());
      _terms.erase(_terms.end() - 1);
    } else
      ++it1;
  }  
}

void TermList::print() const {
  cerr << "//------------ TermList:\n";
  for (const_iterator it = begin(); it != end(); ++it)
    cerr << *it << '\n';
  cerr << "------------\\\\" << endl;
}
