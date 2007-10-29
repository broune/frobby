#include "stdinc.h"
#include "TermTranslator.h"

#include "Term.h"

#include <iterator>
#include <sstream>

TermTranslator::TermTranslator(const VarNames& names,
			       vector<vector<mpz_class> >* decompressionMaps):
  _decompressionMaps(decompressionMaps) {
  ASSERT(decompressionMaps != 0);

  _stringDecompressionMaps.resize(_decompressionMaps->size());
  for (unsigned int i = 0; i < _decompressionMaps->size(); ++i) {
    _stringDecompressionMaps[i].resize((*_decompressionMaps)[i].size());
    for (unsigned int j = 0; j < (*_decompressionMaps)[i].size(); ++j) {
      // TODO: this should be possible without using stringstream - check 
      // the GMP docs.
      char* str = 0;

      if ((*_decompressionMaps)[i][j] != 0) {
	stringstream out;
	out << names.getName(i);
	if ((*_decompressionMaps)[i][j] != 1) 
	  out << '^' << (*_decompressionMaps)[i][j];
      
	str = new char[out.str().size() + 1];
	strcpy(str, out.str().c_str());
      }
      _stringDecompressionMaps[i][j] = str;
    }
  }
}

TermTranslator::~TermTranslator() {
  delete _decompressionMaps;
  for (size_t i = 0; i < _stringDecompressionMaps.size(); ++i)
    for (size_t j = 0; j < _stringDecompressionMaps[i].size(); ++j)
      delete[] _stringDecompressionMaps[i][j];
}

const mpz_class& TermTranslator::
getExponent(int variable, Exponent exponent) const {
  ASSERT(0 <= variable);
  ASSERT(variable < (int)_decompressionMaps->size());
  ASSERT(exponent < (*_decompressionMaps)[variable].size());
  
  return (*_decompressionMaps)[variable][exponent];
}

const char* TermTranslator::
getExponentString(int variable, Exponent exponent) const {
  ASSERT(0 <= variable);
  ASSERT(variable < (int)_decompressionMaps->size());
  ASSERT(exponent < (*_decompressionMaps)[variable].size());

  return (_stringDecompressionMaps[variable][exponent]);
}

const mpz_class& TermTranslator::
getExponent(int variable, const Term& term) const {
  return getExponent(variable, term[variable]);
}

Exponent TermTranslator::getMaxId(int variable) const {
  ASSERT(0 <= variable);
  ASSERT(variable < (int)_decompressionMaps->size());
  
  return (*_decompressionMaps)[variable].size() - 1;
}
