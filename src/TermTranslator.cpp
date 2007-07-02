#include "stdinc.h"
#include "TermTranslator.h"

#include "Term.h"

#include <iterator>

TermTranslator::TermTranslator(vector<vector<mpz_class> >* decompressionMaps):
  _decompressionMaps(decompressionMaps) {
  ASSERT(decompressionMaps != 0);
}

TermTranslator::~TermTranslator() {
  delete _decompressionMaps;
}

const mpz_class& TermTranslator::
getExponent(int variable, Exponent exponent) const {
  ASSERT(0 <= variable);
  ASSERT(variable < (int)_decompressionMaps->size());
  ASSERT(exponent < (*_decompressionMaps)[variable].size());
  
  return (*_decompressionMaps)[variable][exponent];
}

const mpz_class& TermTranslator::
getExponent(int variable, const ExternalTerm& term) const {
  return getExponent(variable, term[variable]);
}

Exponent TermTranslator::getMaxId(int variable) const {
  ASSERT(0 <= variable);
  ASSERT(variable < (int)_decompressionMaps->size());
  
  return (*_decompressionMaps)[variable].size() - 1;
}
