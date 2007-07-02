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
