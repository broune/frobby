#include "stdinc.h"
#include "TermTranslator.h"

#include "Term.h"
#include "Ideal.h"
#include "BigIdeal.h"
#include "VarNames.h"

#include <iterator>
#include <sstream>
#include <set>
#include <algorithm>

TermTranslator::TermTranslator(const BigIdeal& bigIdeal, Ideal& ideal,
			       bool sortVars) {
  vector<BigIdeal*> bigIdeals;
  bigIdeals.push_back((BigIdeal*)&bigIdeal);
  initialize(bigIdeals, sortVars);

  shrinkBigIdeal(bigIdeal, ideal);
}

TermTranslator::TermTranslator(const vector<BigIdeal*>& bigIdeals,
			       vector<Ideal*>& ideals) {
  ASSERT(!bigIdeals.empty());

  initialize(bigIdeals, true);

  for (size_t i = 0; i < bigIdeals.size(); ++i) {
    ideals.push_back(new Ideal());
    shrinkBigIdeal(*(bigIdeals[i]), *(ideals.back()));
  }
}

// Helper function for initialize.
//
// Assign int IDs to big integer exponents. The correspondence
// preserves order, except that the largest ID maps to 0, which is
// necessary to support adding artinian powers. Only the exponents
// that actually appear in generators of the ideals are translated,
// except that 0 is guaranteed to be included and to be assigned the
// ID 0.
void makeExponents(const vector<BigIdeal*>& ideals,
		   vector<mpz_class>& exponents,
		   const string& varName) {
  exponents.clear();
  exponents.push_back(0); // 0 must be included

  // Reserve sufficient capacity for the exponents.
  size_t termCount = 0;
  for (size_t i = 0; i < ideals.size(); ++i)
    termCount += ideals[i]->getGeneratorCount();
  exponents.reserve(termCount);

  // Collect the exponents
  exponents.push_back(0); // Zero must always be present.
  for (size_t ideal = 0; ideal < ideals.size(); ++ideal) {
    size_t var = ideals[ideal]->getNames().getIndex(varName);
    if (var == VarNames::UNKNOWN)
      continue;

    size_t generatorCount = ideals[ideal]->getGeneratorCount();
    for (size_t term = 0; term < generatorCount; ++term)
      exponents.push_back(ideals[ideal]->getExponent(term, var));
  }

  // Sort and remove duplicates.
  sort(exponents.begin(), exponents.end());
  exponents.erase(unique(exponents.begin(), exponents.end()),
		  exponents.end());
  exponents.push_back(0);
}

void TermTranslator::initialize(const vector<BigIdeal*>& bigIdeals,
				bool sortVars) {
  ASSERT(!bigIdeals.empty());

  if (sortVars) {
    set<string> variables;
    for (size_t ideal = 0; ideal < bigIdeals.size(); ++ideal)
      for (size_t var = 0; var < bigIdeals[ideal]->getVarCount(); ++var)
	variables.insert(bigIdeals[ideal]->getNames().getName(var));
    
    for (set<string>::const_iterator var = variables.begin();
	 var != variables.end(); ++var)
      _names.addVar(*var);
  } else {
    ASSERT(bigIdeals.size() == 1);
    _names = bigIdeals[0]->getNames();
  }

  _exponents.resize(_names.getVarCount());

  for (size_t var = 0; var < _names.getVarCount(); ++var)
    makeExponents(bigIdeals, _exponents[var], _names.getName(var));

  makeStrings();
}

void TermTranslator::shrinkBigIdeal(const BigIdeal& bigIdeal,
				    Ideal& ideal) const {
  ideal.clearAndSetVarCount(_names.getVarCount());

  // Figure out how bigIdeal's names map onto _names.
  vector<size_t> newVars;
  for (size_t var = 0; var < bigIdeal.getVarCount(); ++var) {
    const string& name = bigIdeal.getNames().getName(var);
    size_t newVar = _names.getIndex(name);
    newVars.push_back(newVar);

    ASSERT(newVar != VarNames::UNKNOWN);
  }

  // Insert generators after translating exponents and variables.
  Term term(ideal.getVarCount());
  size_t varCount = bigIdeal.getVarCount();
  for (size_t i = 0; i < bigIdeal.getGeneratorCount(); ++i) {
    for (size_t var = 0; var < varCount; ++var) {
      size_t newVar = newVars[var];
      term[newVar] = shrinkExponent(newVar, bigIdeal.getExponent(i, var));
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
    artinian[var] = _exponents[var].size() - 1;
    ideal.insert(artinian);
  }
}

void TermTranslator::print(ostream& out) const {
  out << "TermTranslator(" << endl;
  for (int variable = 0;
       variable < (int)_exponents.size(); ++variable) {
    out << " variable " << (variable + 1) << ": ";
    copy(_exponents[variable].begin(),
	 _exponents[variable].end(),
	 ostream_iterator<mpz_class>(out, " "));
    out << endl;
  }
  out << ")" << endl;
}

void TermTranslator::makeStrings() {
  _stringExponents.resize(_exponents.size());
  for (unsigned int i = 0; i < _exponents.size(); ++i) {
    _stringExponents[i].resize(_exponents[i].size());
    for (unsigned int j = 0; j < _exponents[i].size(); ++j) {
      char* str = 0;

      if (_exponents[i][j] != 0) {
	stringstream out;
	out << _names.getName(i);
	if (_exponents[i][j] != 1) 
	  out << '^' << _exponents[i][j];
      
	str = new char[out.str().size() + 1];
	strcpy(str, out.str().c_str());
      }
      _stringExponents[i][j] = str;
    }
  }
}

TermTranslator::~TermTranslator() {
  for (size_t i = 0; i < _stringExponents.size(); ++i)
    for (size_t j = 0; j < _stringExponents[i].size(); ++j)
      delete[] _stringExponents[i][j];
}

const mpz_class& TermTranslator::
getExponent(int variable, Exponent exponent) const {
  ASSERT(0 <= variable);
  ASSERT(variable < (int)_exponents.size());
  ASSERT(exponent < _exponents[variable].size());
  
  return _exponents[variable][exponent];
}

const char* TermTranslator::
getExponentString(int variable, Exponent exponent) const {
  ASSERT(0 <= variable);
  ASSERT(variable < (int)_exponents.size());
  ASSERT(exponent < _exponents[variable].size());

  return (_stringExponents[variable][exponent]);
}

const mpz_class& TermTranslator::
getExponent(int variable, const Term& term) const {
  return getExponent(variable, term[variable]);
}

Exponent TermTranslator::getMaxId(int variable) const {
  ASSERT(0 <= variable);
  ASSERT(variable < (int)_exponents.size());
  
  return _exponents[variable].size() - 1;
}

Exponent TermTranslator::shrinkExponent(size_t var,
				    const mpz_class& exponent) const {
  const vector<mpz_class>& exponents = _exponents[var];

  // We subtract 1 from exponents.end() to skip past the 0 that is
  // added there. Otherwise the range would not be sorted.
  vector<mpz_class>::const_iterator it =
    lower_bound(exponents.begin(), exponents.end() - 1, exponent);
  ASSERT(*it == exponent);

  return it - exponents.begin();
}

const VarNames& TermTranslator::getNames() const {
  return _names;
}
