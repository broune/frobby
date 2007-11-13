#include "stdinc.h"
#include "TermTranslator.h"

#include "Term.h"

#include <iterator>
#include <sstream>

#include "Ideal.h"
#include "BigIdeal.h"

void collectAndSortExponents(const vector<BigIdeal*>& ideals,
			     vector<mpz_class>& exponents,
			     size_t var) {
  exponents.clear();

  // Reserve sufficient capacity for the exponents.
  size_t termCount = 0;
  for (size_t i = 0; i < ideals.size(); ++i)
    termCount += ideals[i]->getGeneratorCount();
  exponents.reserve(termCount);

  // Collect the exponents
  exponents.push_back(0); // Zero must always be present.
  for (size_t ideal = 0; ideal < ideals.size(); ++ideal) {
    size_t generatorCount = ideals[ideal]->getGeneratorCount();
    for (size_t term = 0; term < generatorCount; ++term)
      exponents.push_back(ideals[ideal]->getExponent(term, var));
  }

  // Sort and remove duplicates.
  sort(exponents.begin(), exponents.end());
  exponents.erase(unique(exponents.begin(), exponents.end()), exponents.end());
}

void makeCompressionMap
(vector<mpz_class>& exponents,
 map<mpz_class, Exponent>& compressionMap) {

  // Construct the map from large exponents to small id numbers. The
  // map preserves order.
  compressionMap.clear();
  compressionMap[0] = 0;
  Exponent maxId = 0;

  size_t startingIndex = (exponents[0] == 0 ? 1 : 0);
  for (size_t i = startingIndex; i < exponents.size(); ++i)
    compressionMap[exponents[i]] = ++maxId;
}

TermTranslator::TermTranslator(const BigIdeal& bigIdeal) {
  vector<BigIdeal*> bigIdeals;
  bigIdeals.push_back((BigIdeal*)&bigIdeal);
  initialize(bigIdeals);
}

TermTranslator::TermTranslator(const vector<BigIdeal*>& bigIdeals) {
  initialize(bigIdeals);
}

void TermTranslator::initialize(const vector<BigIdeal*>& bigIdeals) {
  ASSERT(!bigIdeals.empty());
  size_t varCount = bigIdeals[0]->getVarCount();

#ifdef DEBUG
  for (size_t i = 0; i < bigIdeals.size(); ++i)
    ASSERT(bigIdeals[i]->getVarCount() == varCount);
#endif

  _compressionMaps.resize(varCount);

  _decompressionMaps = new vector<vector<mpz_class> >(varCount);
  for (size_t var = 0; var < varCount; ++var) {
    collectAndSortExponents(bigIdeals, (*_decompressionMaps)[var], var);
    makeCompressionMap((*_decompressionMaps)[var], _compressionMaps[var]);
    (*_decompressionMaps)[var].push_back(0);
  }

  makeStrings(bigIdeals[0]->getNames());
}


void TermTranslator::shrinkBigIdeal
(const BigIdeal& bigIdeal, Ideal& ideal) const {
  size_t varCount = bigIdeal.getVarCount();

  ideal.clearAndSetVarCount(varCount);

  Term term(varCount);
  for (size_t i = 0; i < bigIdeal.getGeneratorCount(); ++i) {
    for (size_t var = 0; var < varCount; ++var) {
      map<mpz_class, Exponent>::const_iterator it =
	_compressionMaps[var].find(bigIdeal.getExponent(i, var));
      ASSERT(it != _compressionMaps[var].end());
      term[var] = it->second;
    }
    ideal.insert(term);
  }
}

void TermTranslator::addArtinianPowers(Ideal& ideal) const {
  size_t varCount = ideal.getVarCount();

  // Find out which variables already have artinian powers.
  vector<bool> hasArtinianPower(varCount);
  
  Ideal::const_iterator stop = ideal.end();
  for (Ideal::const_iterator term = ideal.begin(); term != stop; ++term) {
    if (getSizeOfSupport(*term, varCount) > 1)
      continue;

    size_t var = getFirstNonZeroExponent(*term, varCount);
    if (var == varCount)
      return; // The ideal is <1> so we need add nothing.

    hasArtinianPower[var] = true;
  }

  // Add any missing Artinian powers.
  for (size_t var = 0; var < varCount; ++var) {
    if (hasArtinianPower[var])
      continue;

    Term artinian(varCount);
    artinian[var] = (*_decompressionMaps)[var].size() - 1;
    ideal.insert(artinian);
  }
}

void TermTranslator::print(ostream& out) const {
  out << "TermTranslator(" << endl;
  for (int variable = 0;
       variable < (int)_decompressionMaps->size(); ++variable) {
    out << " variable " << (variable + 1) << ": ";
    copy((*(_decompressionMaps))[variable].begin(),
	 (*(_decompressionMaps))[variable].end(),
	 ostream_iterator<mpz_class>(out, " "));
    out << endl;
  }
  out << ")" << endl;
}

TermTranslator::TermTranslator(const VarNames& names,
			       vector<vector<mpz_class> >* decompressionMaps):
  _decompressionMaps(decompressionMaps) {
  ASSERT(decompressionMaps != 0);

  makeStrings(names);
}

void TermTranslator::makeStrings(const VarNames& names) {
  _stringDecompressionMaps.resize(_decompressionMaps->size());
  for (unsigned int i = 0; i < _decompressionMaps->size(); ++i) {
    _stringDecompressionMaps[i].resize((*_decompressionMaps)[i].size());
    for (unsigned int j = 0; j < (*_decompressionMaps)[i].size(); ++j) {
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
