#ifndef TERM_LIST_GUARD
#define TERM_LIST_GUARD

#include "Term.h"

class TermList {
  typedef vector<ExternalTerm> Cont;

public:
  typedef Cont::iterator iterator;
  typedef Cont::const_iterator const_iterator;

  TermList(unsigned int varCount):
    _varCount(varCount) {
  }

  void insert(const ExternalTerm& term) {
    _terms.push_back(term);
  }

  const_iterator begin() const {
    return _terms.begin();
  }

  const_iterator end() const {
    return _terms.end();
  }

  bool isIncomparable(const ExternalTerm& term) const {
    for (const_iterator it = begin(); it != end(); ++it)
      if (it->divides(term) || term.divides(*it))
	return false;
    return true;
  }

  size_t size() const {
    return _terms.size();
  }

private:
  Cont _terms;
  unsigned int _varCount;
};

#endif
